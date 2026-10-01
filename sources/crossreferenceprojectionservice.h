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
#ifndef CROSSREFERENCEPROJECTIONSERVICE_H
#define CROSSREFERENCEPROJECTIONSERVICE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QUuid>

class QETProject;

struct CrossReferenceProjection
{
	QString kind;
	QString source_role;
	QUuid source_uuid;
	QString source_label;
	int source_folio = -1;
	QString target_role;
	QUuid target_uuid;
	QString target_label;
	int target_folio = -1;
	QString relationship;
	QString cardinality;
	QString display_text;
	QString reference_text;
	QString status;
	QStringList warnings;
	QString source_service;
};

class CrossReferenceProjectionService
{
	public:
		QList<CrossReferenceProjection> references(QETProject &project) const;
};

#endif // CROSSREFERENCEPROJECTIONSERVICE_H
