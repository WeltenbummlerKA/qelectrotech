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
#ifndef CONTINUATIONPROJECTIONSERVICE_H
#define CONTINUATIONPROJECTIONSERVICE_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QUuid>

class QETProject;

struct ContinuationProjection
{
	QUuid uuid;
	QString potential;
	QString signal;
	QString voltage;
	QString mam_continuation_id;
	QString mam_pair_id;
	QString mam_chain_id;
	int mam_chain_order = -1;
	QString chain;
	int chain_order = -1;
	QString direction;
	int folio = -1;
	QString grid;
	QString path;
	QString visible_xref;
	QUuid target_uuid;
	int target_folio = -1;
	QString target_grid;
	QString target_path;
	QString computed_xref;
	QString cardinality;
	QString relationship;
	QString status;
	QStringList diagnostics;
	QString suggested_mam_pair_id;
	QString migration_recommendation;
};

class ContinuationProjectionService
{
	public:
		QList<ContinuationProjection> continuations(QETProject &project) const;
};

#endif // CONTINUATIONPROJECTIONSERVICE_H
