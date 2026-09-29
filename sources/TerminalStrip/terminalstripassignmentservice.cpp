/*
	Copyright 2006-2026 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#include "terminalstripassignmentservice.h"

#include "../elementprovider.h"
#include "../qetgraphicsitem/terminalelement.h"
#include "../qetproject.h"
#include "physicalterminal.h"
#include "realterminal.h"
#include "terminalstrip.h"

#include <algorithm>
#include <utility>

namespace {

struct ParsedTerminalLabel
{
	bool matches = false;
	qulonglong number = 0;
	QString suffix;
};

ParsedTerminalLabel parseSingleLevelTerminalLabel(const QString &label, const QString &prefix)
{
	const QString separator = prefix + QLatin1Char(':');
	if (!label.startsWith(separator))
		return {};

	const QString suffix = label.mid(separator.size());
	if (suffix.isEmpty())
		return {};

	for (const QChar ch : suffix) {
		if (!ch.isDigit())
			return {};
	}

	bool ok = false;
	const qulonglong number = suffix.toULongLong(&ok);
	if (!ok)
		return {};

	return {true, number, suffix};
}

struct FreeTerminalMatch
{
	TerminalElement *terminal = nullptr;
	qulonglong number = 0;
	QString label;
	QString uuid;
};

struct PhysicalTerminalOrder
{
	QSharedPointer<PhysicalTerminal> physical_terminal;
	bool matches = false;
	qulonglong number = 0;
	QString label;
	QString uuid;
	int original_index = 0;
};

TerminalStrip *terminalStripByName(QETProject *project, const QString &name)
{
	for (TerminalStrip *strip : project->terminalStrip()) {
		if (strip && strip->name() == name)
			return strip;
	}
	return nullptr;
}

bool freeTerminalLessThan(const FreeTerminalMatch &left, const FreeTerminalMatch &right)
{
	if (left.number != right.number)
		return left.number < right.number;
	if (left.label != right.label)
		return left.label < right.label;
	return left.uuid < right.uuid;
}

bool physicalTerminalLessThan(const PhysicalTerminalOrder &left, const PhysicalTerminalOrder &right)
{
	if (left.number != right.number)
		return left.number < right.number;
	if (left.label != right.label)
		return left.label < right.label;
	return left.uuid < right.uuid;
}

} // anonymous namespace

TerminalStripAssignmentService::Result TerminalStripAssignmentService::assignSingleLevelByPrefix(
	QETProject *project,
	const QString &prefix)
{
	Result result;
	if (!project || prefix.isEmpty())
		return result;

	QVector<FreeTerminalMatch> free_matches;
	const ElementProvider provider(project);
	for (TerminalElement *terminal : provider.freeTerminal())
	{
		if (!terminal)
			continue;

		const QString label = terminal->actualLabel();
		const ParsedTerminalLabel parsed = parseSingleLevelTerminalLabel(label, prefix);
		if (!parsed.matches)
			continue;

		free_matches.append({
			terminal,
			parsed.number,
			label,
			terminal->uuid().toString()
		});
	}

	std::sort(free_matches.begin(), free_matches.end(), freeTerminalLessThan);
	result.matched_free_terminals = free_matches.size();

	TerminalStrip *strip = terminalStripByName(project, prefix);
	if (!strip && free_matches.isEmpty())
		return result;

	if (!strip) {
		strip = project->newTerminalStrip(QString(), QString(), prefix);
		result.created_strip = true;
	}
	result.strip = strip;

	bool existing_strip_is_simple = true;
	for (const auto &physical : strip->physicalTerminal())
	{
		if (!physical || physical->realTerminalCount() != 1)
		{
			existing_strip_is_simple = false;
			break;
		}

		const QSharedPointer<RealTerminal> real = physical->realTerminal(0);
		const ParsedTerminalLabel parsed = real
			? parseSingleLevelTerminalLabel(real->label(), prefix)
			: ParsedTerminalLabel();
		if (!parsed.matches)
		{
			existing_strip_is_simple = false;
			break;
		}
	}
	if (!existing_strip_is_simple) {
		result.skipped_non_single_level_strip = true;
		return result;
	}

	QVector<QSharedPointer<RealTerminal>> terminals_to_add;
	for (const FreeTerminalMatch &match : std::as_const(free_matches)) {
		if (match.terminal && match.terminal->realTerminal()) {
			terminals_to_add.append(match.terminal->realTerminal());
		}
	}

	const int before_count = strip->physicalTerminalCount();
	strip->addTerminals(terminals_to_add);
	result.added_terminals = strip->physicalTerminalCount() - before_count;

	QVector<PhysicalTerminalOrder> ordered;
	const QVector<QSharedPointer<PhysicalTerminal>> physical_terminals = strip->physicalTerminal();
	ordered.reserve(physical_terminals.size());
	for (int index = 0; index < physical_terminals.size(); ++index)
	{
		const QSharedPointer<PhysicalTerminal> physical = physical_terminals.at(index);
		PhysicalTerminalOrder entry;
		entry.physical_terminal = physical;
		entry.original_index = index;

		if (physical && physical->realTerminalCount() == 1)
		{
			const QSharedPointer<RealTerminal> real = physical->realTerminal(0);
			if (real)
			{
				const QString label = real->label();
				const ParsedTerminalLabel parsed = parseSingleLevelTerminalLabel(label, prefix);
				if (parsed.matches)
				{
					entry.matches = true;
					entry.number = parsed.number;
					entry.label = label;
					entry.uuid = real->elementUuid().toString();
					++result.matching_strip_terminals;
				}
			}
		}

		ordered.append(entry);
	}

	std::sort(ordered.begin(), ordered.end(), physicalTerminalLessThan);

	QVector<QSharedPointer<PhysicalTerminal>> sorted_physical_terminals;
	sorted_physical_terminals.reserve(ordered.size());
	for (const PhysicalTerminalOrder &entry : std::as_const(ordered)) {
		sorted_physical_terminals.append(entry.physical_terminal);
	}

	if (sorted_physical_terminals != physical_terminals) {
		result.reordered = strip->setOrderTo(sorted_physical_terminals);
	}
	return result;
}
