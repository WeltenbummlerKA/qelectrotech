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

#include "contactcrossrefprojectionservice.h"
#include "plcioprojectionservice.h"

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

} // namespace

QList<CrossReferenceProjection> CrossReferenceProjectionService::references(QETProject &project) const
{
	QList<CrossReferenceProjection> result;

	ContactCrossRefProjectionService contact_service;
	for (const ContactAssignmentProjection &assignment : contact_service.assignments(project)) {
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

	return result;
}
