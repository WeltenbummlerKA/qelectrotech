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
#ifndef CABLECORECROSSREFPROJECTIONSERVICE_H
#define CABLECORECROSSREFPROJECTIONSERVICE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QUuid>

class QETProject;

struct CableCoreCrossRefProjection
{
	QUuid conductor_uuid;
	QString cable;
	QString wire_number;
	QString wire_color;
	QString wire_section;
	QString function;
	QUuid source_element_uuid;
	QString source_element_label;
	QString source_terminal;
	int source_folio = -1;
	QString source_grid;
	QString source_reference;
	QUuid target_element_uuid;
	QString target_element_label;
	QString target_terminal;
	int target_folio = -1;
	QString target_grid;
	QString target_reference;
	QString display_text;
	QString relationship;
	QString reference_text;
	QStringList warnings;
};

class CableCoreCrossRefProjectionService
{
	public:
		QList<CableCoreCrossRefProjection> references(QETProject &project) const;
};

#endif // CABLECORECROSSREFPROJECTIONSERVICE_H
