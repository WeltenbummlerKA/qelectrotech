/*
	Copyright 2006-2026 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#include "crossreferenceprojectionservice.h"

#include "autoNum/assignvariables.h"
#include "cablecorecrossrefprojectionservice.h"
#include "contactcrossrefprojectionservice.h"
#include "continuationprojectionservice.h"
#include "diagram.h"
#include "diagramposition.h"
#include "plcioprojectionservice.h"
#include "qetgraphicsitem/element.h"
#include "qetproject.h"
#include "TerminalStrip/physicalterminal.h"
#include "TerminalStrip/realterminal.h"
#include "TerminalStrip/terminalstrip.h"
#include "TerminalStrip/terminalstripbridge.h"

#include <QHash>
#include <QPointF>

#include <algorithm>

namespace {

QString folioReference(int folio)
{
	return folio < 0 ? QString() : QStringLiteral("/%1").arg(folio);
}

QString statusFor(const QStringList &warnings)
{
	return warnings.isEmpty() ? QStringLiteral("OK") : QStringLiteral("WARNING");
}

QString plcReferenceText(const PlcIoProjection &channel)
{
	QStringList parts {
		QStringLiteral("io_index %1").arg(channel.io_index)
	};
	if (!channel.address.isEmpty()) {
		parts << channel.address;
	}
	if (!channel.terminal_labels.isEmpty()) {
		parts << channel.terminal_labels.join(QLatin1Char(','));
	}
	return parts.join(QStringLiteral(" | "));
}

QString elementLabel(Element *element)
{
	if (!element) {
		return QString();
	}

	const QString label = element->elementInformations().value(QStringLiteral("label")).toString();
	return label.isEmpty() ? element->name() : label;
}

QString elementPosition(Element *element)
{
	Diagram *diagram = element ? element->diagram() : nullptr;
	if (!diagram) {
		return QString();
	}

	return diagram->convertPosition(element->scenePos()).toString();
}

QString uuidString(const QUuid &uuid)
{
	return uuid.isNull() ? QString() : uuid.toString(QUuid::WithoutBraces);
}

QString reportRole(Element *element)
{
	if (!element) {
		return QString();
	}

	if (element->linkType() == Element::NextReport) {
		return QStringLiteral("next_report");
	}
	if (element->linkType() == Element::PreviousReport) {
		return QStringLiteral("previous_report");
	}
	return QStringLiteral("report");
}

QString reportRelationship(Element *source, Element *target)
{
	return reportRole(source) % QStringLiteral("_to_") % reportRole(target);
}

QString reportDisplayText(QETProject &project, Element *target)
{
	if (!target || !target->diagram()) {
		return QString();
	}

	autonum::sequentialNumbers sequence = target->sequenceStruct();
	return autonum::AssignVariables::formulaToLabel(
		project.defaultReportProperties(),
		sequence,
		target->diagram(),
		target);
}

QString reportReferenceText(Element *target)
{
	if (!target || !target->diagram()) {
		return QString();
	}

	const QString position = elementPosition(target);
	if (position.isEmpty()) {
		return QString();
	}
	return QStringLiteral("/%1.%2").arg(target->diagram()->folioIndex()).arg(position);
}

bool isReportPair(Element *source, Element *target)
{
	if (!source || !target) {
		return false;
	}
	if (!(source->linkType() & Element::AllReport) || !(target->linkType() & Element::AllReport)) {
		return false;
	}
	return source->linkType() != target->linkType();
}

QHash<Element *, int> folioIndex(QETProject &project)
{
	QHash<Element *, int> folio;
	int index = 0;
	const QList<Diagram *> diagrams = project.diagrams();
	for (Diagram *diagram : diagrams) {
		++index;
		const QList<Element *> elements = diagram->elements();
		for (Element *element : elements) {
			folio.insert(element, index);
		}
	}
	return folio;
}

QString terminalRelationship(
		int physical_index,
		const QSharedPointer<PhysicalTerminal> &physical,
		const QSharedPointer<RealTerminal> &real)
{
	QStringList parts {
		QStringLiteral("physical_index %1").arg(physical_index + 1),
		QStringLiteral("level %1").arg(real ? real->level() + 1 : 0)
	};

	if (physical) {
		parts << QStringLiteral("level_count %1").arg(physical->levelCount());
	}
	if (real && !real->label().isEmpty()) {
		parts << QStringLiteral("terminal_label %1").arg(real->label());
	}
	if (real) {
		const QSharedPointer<TerminalStripBridge> bridge = real->bridge();
		if (bridge) {
			parts << QStringLiteral("bridge %1").arg(uuidString(bridge->uuid()));
		}
	}

	return parts.join(QStringLiteral(" | "));
}

QString terminalSourcePosition(int physical_index, const QSharedPointer<RealTerminal> &real)
{
	QStringList parts {
		QStringLiteral("physical %1").arg(physical_index + 1)
	};
	if (real) {
		parts << QStringLiteral("level %1").arg(real->level() + 1);
	}
	return parts.join(QStringLiteral(" | "));
}

QString terminalReferenceText(
		TerminalStrip *strip,
		int physical_index,
		const QSharedPointer<PhysicalTerminal> &physical,
		const QSharedPointer<RealTerminal> &real)
{
	QStringList parts;
	if (strip) {
		parts << strip->name();
	}
	parts << QStringLiteral("physical %1").arg(physical_index + 1);
	if (real) {
		parts << QStringLiteral("level %1").arg(real->level() + 1);
		if (!real->label().isEmpty()) {
			parts << real->label();
		}
		if (!real->Xref().isEmpty()) {
			parts << real->Xref();
		}
	}
	if (physical) {
		parts << QStringLiteral("%1 level(s)").arg(physical->levelCount());
	}
	return parts.join(QStringLiteral(" | "));
}

} // namespace

QList<CrossReferenceProjection> CrossReferenceProjectionService::references(QETProject &project) const
{
	QList<CrossReferenceProjection> result;

	ContactCrossRefProjectionService contact_service;
	QHash<QUuid, ContactMasterProjection> contact_masters;
	for (const ContactMasterProjection &master : contact_service.masters(project)) {
		contact_masters.insert(master.uuid, master);
	}

	for (const ContactAssignmentProjection &assignment : contact_service.assignments(project)) {
		const ContactMasterProjection master = contact_masters.value(assignment.master_uuid);
		if (master.master_type == ElementData::PLC) {
			continue;
		}

		CrossReferenceProjection reference;
		reference.kind = QStringLiteral("contact_crossref");
		reference.source_role = QStringLiteral("master");
		reference.source_uuid = assignment.master_uuid;
		reference.source_label = assignment.master_label;
		reference.source_folio = assignment.master_folio;
		reference.target_role = QStringLiteral("slave_contact");
		reference.target_uuid = assignment.slave_uuid;
		reference.target_label = assignment.slave_label;
		reference.target_folio = assignment.slave_folio;
		reference.relationship = assignment.group_index >= 0
			? QStringLiteral("group_index %1").arg(assignment.group_index)
			: QStringLiteral("missing group_index");
		reference.cardinality = QStringLiteral("1:n");
		reference.display_text = folioReference(assignment.slave_folio);
		reference.reference_text = reference.relationship;
		reference.warnings = assignment.validation_messages;
		reference.status = statusFor(reference.warnings);
		reference.source_service = QStringLiteral("ContactCrossRefProjectionService");
		result << reference;
	}

	PlcIoProjectionService plc_service;
	for (const PlcIoProjection &channel : plc_service.channels(project)) {
		CrossReferenceProjection reference;
		reference.kind = QStringLiteral("plc_io");
		reference.source_role = QStringLiteral("plc_master_channel");
		reference.source_uuid = channel.master_uuid;
		reference.source_label = channel.master_label;
		reference.source_folio = channel.folio;
		reference.target_role = QStringLiteral("plc_slave");
		reference.target_uuid = channel.linked_slave_uuid;
		reference.target_label = channel.linked_slave_label;
		reference.target_folio = channel.linked_slave_folio;
		reference.relationship = QStringLiteral("io_index %1").arg(channel.io_index);
		reference.cardinality = QStringLiteral("1:1");
		reference.display_text = folioReference(channel.linked_slave_folio);
		reference.reference_text = plcReferenceText(channel);
		reference.warnings = channel.warnings;
		reference.status = statusFor(reference.warnings);
		reference.source_service = QStringLiteral("PlcIoProjectionService");
		result << reference;
	}

	CableCoreCrossRefProjectionService cable_service;
	for (const CableCoreCrossRefProjection &cable_reference : cable_service.references(project)) {
		CrossReferenceProjection reference;
		reference.kind = QStringLiteral("cable_plan_reference");
		reference.source_role = QStringLiteral("cable_core");
		reference.source_uuid = cable_reference.conductor_uuid;
		reference.source_label = cable_reference.wire_number.isEmpty()
			? cable_reference.cable
			: cable_reference.cable % QLatin1Char(':') % cable_reference.wire_number;
		reference.source_folio = cable_reference.source_folio;
		reference.source_position = cable_reference.source_reference;
		reference.target_role = QStringLiteral("cable_plan_row");
		reference.target_uuid = cable_reference.conductor_uuid;
		reference.target_label = cable_reference.cable;
		reference.target_folio = -1;
		reference.target_position = QStringLiteral("source %1 | target %2")
			.arg(cable_reference.source_reference, cable_reference.target_reference);
		reference.relationship = cable_reference.relationship;
		reference.cardinality = QStringLiteral("1:1");
		reference.display_text = cable_reference.display_text;
		reference.reference_text = cable_reference.reference_text;
		reference.warnings = cable_reference.warnings;
		reference.status = statusFor(reference.warnings);
		reference.source_service = QStringLiteral("CableCoreCrossRefProjectionService");
		result << reference;
	}

	const QList<Diagram *> diagrams = project.diagrams();
	for (Diagram *diagram : diagrams) {
		if (!diagram) {
			continue;
		}

		const QList<Element *> elements = diagram->elements();
		for (Element *element : elements) {
			if (!element || !(element->linkType() & Element::AllReport)) {
				continue;
			}

			const QList<Element *> linked_elements = element->linkedElementsReadOnly();
			if (linked_elements.size() != 1) {
				continue;
			}

			Element *target = linked_elements.first();
			if (!isReportPair(element, target)) {
				continue;
			}

			CrossReferenceProjection reference;
			reference.kind = QStringLiteral("report_folio_backreference");
			reference.source_role = reportRole(element);
			reference.source_uuid = element->uuid();
			reference.source_label = elementLabel(element);
			reference.source_folio = element->diagram()
				? element->diagram()->folioIndex()
				: -1;
			reference.source_position = elementPosition(element);
			reference.target_role = reportRole(target);
			reference.target_uuid = target->uuid();
			reference.target_label = elementLabel(target);
			reference.target_folio = target->diagram()
				? target->diagram()->folioIndex()
				: -1;
			reference.target_position = elementPosition(target);
			reference.relationship = reportRelationship(element, target);
			reference.cardinality = QStringLiteral("1:1");
			reference.display_text = reportDisplayText(project, target);
			reference.reference_text = reportReferenceText(target);
			reference.status = QStringLiteral("OK");
			reference.source_service = QStringLiteral("ReportElement");
			result << reference;
		}
	}

	const QHash<Element *, int> folios = folioIndex(project);
	QVector<TerminalStrip *> strips = project.terminalStrip();
	std::sort(strips.begin(), strips.end(), [](TerminalStrip *left, TerminalStrip *right) {
		const QString left_key = left->installation()
			% QLatin1Char('|') % left->location()
			% QLatin1Char('|') % left->name()
			% QLatin1Char('|') % uuidString(left->uuid());
		const QString right_key = right->installation()
			% QLatin1Char('|') % right->location()
			% QLatin1Char('|') % right->name()
			% QLatin1Char('|') % uuidString(right->uuid());
		return left_key < right_key;
	});

	for (TerminalStrip *strip : strips) {
		if (!strip) {
			continue;
		}

		for (int physical_index = 0; physical_index < strip->physicalTerminalCount(); ++physical_index) {
			const QSharedPointer<PhysicalTerminal> physical = strip->physicalTerminal(physical_index);
			if (!physical) {
				continue;
			}

			for (const QSharedPointer<RealTerminal> &real : physical->realTerminals()) {
				if (!real) {
					continue;
				}

				Element *element = real->element();
				CrossReferenceProjection reference;
				reference.kind = QStringLiteral("terminal_backreference");
				reference.source_role = QStringLiteral("terminal_strip");
				reference.source_uuid = strip->uuid();
				reference.source_label = strip->name();
				reference.source_folio = -1;
				reference.source_position = terminalSourcePosition(physical_index, real);
				reference.target_role = QStringLiteral("schematic_terminal");
				reference.target_uuid = real->elementUuid();
				reference.target_label = real->label();
				reference.target_folio = element ? folios.value(element, -1) : -1;
				reference.target_position = elementPosition(element);
				reference.relationship = terminalRelationship(physical_index, physical, real);
				reference.cardinality = QStringLiteral("1:n");
				reference.display_text = real->Xref();
				reference.reference_text = terminalReferenceText(strip, physical_index, physical, real);
				if (strip->name().isEmpty()) {
					reference.warnings << QStringLiteral("empty strip_name");
				}
				if (real->elementUuid().isNull()) {
					reference.warnings << QStringLiteral("empty terminal_uuid");
				}
				if (!element) {
					reference.warnings << QStringLiteral("missing terminal element");
				}
				if (reference.display_text.isEmpty()) {
					reference.warnings << QStringLiteral("empty terminal_xref");
				}
				reference.status = statusFor(reference.warnings);
				reference.source_service = QStringLiteral("TerminalStrip");
				result << reference;
			}
		}
	}

	ContinuationProjectionService continuation_service;
	for (const ContinuationProjection &continuation : continuation_service.continuations(project)) {
		CrossReferenceProjection reference;
		reference.kind = QStringLiteral("potential_continuation");
		reference.source_role = QStringLiteral("continuation");
		reference.source_uuid = continuation.uuid;
		reference.source_label = continuation.potential.isEmpty()
			? continuation.signal
			: continuation.potential;
		reference.source_folio = continuation.folio;
		reference.source_position = continuation.grid;
		reference.target_role = QStringLiteral("continuation_target");
		reference.target_uuid = continuation.target_uuid;
		reference.target_label = reference.source_label;
		reference.target_folio = continuation.target_folio;
		reference.target_position = continuation.target_grid;
		reference.relationship = continuation.relationship;
		reference.cardinality = continuation.cardinality;
		reference.display_text = continuation.computed_xref;
		reference.reference_text = continuation.visible_xref;
		reference.warnings = continuation.diagnostics;
		reference.status = continuation.status;
		reference.source_service = QStringLiteral("ContinuationProjectionService");
		result << reference;
	}

	return result;
}
