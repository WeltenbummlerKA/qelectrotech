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
#include "cablecorecrossrefprojectionservice.h"

#include "diagram.h"
#include "diagramcontext.h"
#include "diagramposition.h"
#include "qetgraphicsitem/conductor.h"
#include "qetgraphicsitem/element.h"
#include "qetgraphicsitem/terminal.h"
#include "qetproject.h"

#include <QHash>
#include <QRegularExpression>

#include <algorithm>

namespace {

QHash<Diagram *, int> diagramFolioIndex(QETProject &project)
{
	QHash<Diagram *, int> folios;
	for (Diagram *diagram : project.diagrams()) {
		folios.insert(diagram, project.folioIndex(diagram));
	}
	return folios;
}

QString uuidString(const QUuid &uuid)
{
	return uuid.isNull() ? QString() : uuid.toString(QUuid::WithoutBraces);
}

QString elementLabel(Element *element)
{
	if (!element) {
		return QString();
	}

	const QString label = element->elementInformations().value(QStringLiteral("label")).toString();
	return label.isEmpty() ? element->name() : label;
}

QString terminalName(Terminal *terminal)
{
	return terminal ? terminal->name() : QString();
}

QString terminalGrid(Terminal *terminal)
{
	if (!terminal || !terminal->diagram()) {
		return QString();
	}

	return terminal->diagram()->convertPosition(terminal->dockConductor()).toString();
}

QString gridColumn(const QString &grid)
{
	static const QRegularExpression column_re(QStringLiteral("(\\d+)$"));
	const QRegularExpressionMatch match = column_re.match(grid);
	return match.hasMatch() ? match.captured(1) : QString();
}

QString pathReference(int folio, const QString &grid)
{
	const QString path = gridColumn(grid);
	if (folio < 0 || path.isEmpty()) {
		return QString();
	}
	return QStringLiteral("/%1.%2").arg(folio).arg(path);
}

QString endpointText(const QString &label, const QString &terminal)
{
	if (terminal.isEmpty()) {
		return label;
	}
	if (label.isEmpty()) {
		return terminal;
	}
	return label % QLatin1Char(':') % terminal;
}

QStringList coreEvidence(const ConductorProperties &properties)
{
	QStringList evidence;
	if (!properties.text.isEmpty()) {
		evidence << QStringLiteral("wire %1").arg(properties.text);
	}
	if (!properties.m_wire_color.isEmpty()) {
		evidence << QStringLiteral("color %1").arg(properties.m_wire_color);
	}
	if (!properties.m_wire_section.isEmpty()) {
		evidence << QStringLiteral("section %1").arg(properties.m_wire_section);
	}
	if (!properties.m_function.isEmpty()) {
		evidence << QStringLiteral("function %1").arg(properties.m_function);
	}
	return evidence;
}

} // namespace

QList<CableCoreCrossRefProjection> CableCoreCrossRefProjectionService::references(QETProject &project) const
{
	const QHash<Diagram *, int> folios = diagramFolioIndex(project);

	QList<Conductor *> conductors;
	for (Diagram *diagram : project.diagrams()) {
		conductors << diagram->conductors();
	}

	std::sort(conductors.begin(), conductors.end(), [](Conductor *left, Conductor *right) {
		const QString left_key = left->properties().m_cable
			% QLatin1Char('|') % left->properties().text
			% QLatin1Char('|') % uuidString(left->uuid());
		const QString right_key = right->properties().m_cable
			% QLatin1Char('|') % right->properties().text
			% QLatin1Char('|') % uuidString(right->uuid());
		return left_key < right_key;
	});

	QList<CableCoreCrossRefProjection> result;
	for (Conductor *conductor : conductors) {
		if (!conductor) {
			continue;
		}

		const ConductorProperties properties = conductor->properties();
		if (properties.m_cable.trimmed().isEmpty()) {
			continue;
		}

		Element *source_element = conductor->terminal1 ? conductor->terminal1->parentElement() : nullptr;
		Element *target_element = conductor->terminal2 ? conductor->terminal2->parentElement() : nullptr;
		const int source_folio = folios.value(conductor->terminal1 ? conductor->terminal1->diagram() : nullptr, -1);
		const int target_folio = folios.value(conductor->terminal2 ? conductor->terminal2->diagram() : nullptr, -1);
		const QString source_grid = terminalGrid(conductor->terminal1);
		const QString target_grid = terminalGrid(conductor->terminal2);
		const QString source_terminal = terminalName(conductor->terminal1);
		const QString target_terminal = terminalName(conductor->terminal2);
		const QString source_label = elementLabel(source_element);
		const QString target_label = elementLabel(target_element);
		const QString source_reference = pathReference(source_folio, source_grid);
		const QString target_reference = pathReference(target_folio, target_grid);

		CableCoreCrossRefProjection reference;
		reference.conductor_uuid = conductor->uuid();
		reference.cable = properties.m_cable.trimmed();
		reference.wire_number = properties.text;
		reference.wire_color = properties.m_wire_color;
		reference.wire_section = properties.m_wire_section;
		reference.function = properties.m_function;
		reference.source_element_uuid = source_element ? source_element->uuid() : QUuid();
		reference.source_element_label = source_label;
		reference.source_terminal = source_terminal;
		reference.source_folio = source_folio;
		reference.source_grid = source_grid;
		reference.source_reference = source_reference;
		reference.target_element_uuid = target_element ? target_element->uuid() : QUuid();
		reference.target_element_label = target_label;
		reference.target_terminal = target_terminal;
		reference.target_folio = target_folio;
		reference.target_grid = target_grid;
		reference.target_reference = target_reference;
		reference.display_text = reference.wire_number.trimmed().isEmpty()
			? QStringLiteral("cable_plan:%1").arg(reference.cable)
			: QStringLiteral("cable_plan:%1:%2").arg(reference.cable, reference.wire_number.trimmed());
		reference.relationship = QStringLiteral("cable_plan_row | conductor %1 | cable %2")
			.arg(uuidString(conductor->uuid()), reference.cable);

		QStringList reference_parts {
			QStringLiteral("cable %1").arg(reference.cable)
		};
		reference_parts << coreEvidence(properties);
		reference_parts << QStringLiteral("source %1").arg(
			endpointText(source_label, source_terminal));
		reference_parts << QStringLiteral("target %1").arg(
			endpointText(target_label, target_terminal));
		if (!source_reference.isEmpty()) {
			reference_parts << QStringLiteral("source_reference %1").arg(source_reference);
		}
		if (!target_reference.isEmpty()) {
			reference_parts << QStringLiteral("target_reference %1").arg(target_reference);
		}
		reference_parts << QStringLiteral("source_target %1 -> %2").arg(
			endpointText(source_label, source_terminal),
			endpointText(target_label, target_terminal));
		reference.reference_text = reference_parts.join(QStringLiteral(" | "));

		if (conductor->uuid().isNull()) {
			reference.warnings << QStringLiteral("empty conductor_uuid");
		}
		if (!conductor->terminal1 || !conductor->terminal2) {
			reference.warnings << QStringLiteral("missing conductor endpoint");
		}
		if (!source_element || source_label.isEmpty()) {
			reference.warnings << QStringLiteral("empty source endpoint");
		}
		if (!target_element || target_label.isEmpty()) {
			reference.warnings << QStringLiteral("empty target endpoint");
		}
		if (reference.wire_number.trimmed().isEmpty()
			&& reference.wire_color.trimmed().isEmpty()
			&& reference.wire_section.trimmed().isEmpty()) {
			reference.warnings << QStringLiteral("missing cable core evidence");
		}

		result << reference;
	}

	return result;
}
