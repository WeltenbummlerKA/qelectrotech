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
#ifndef CONTINUATIONPAIRIDASSIGNMENTSERVICE_H
#define CONTINUATIONPAIRIDASSIGNMENTSERVICE_H

class QETProject;

struct ContinuationPairIdAssignmentResult
{
	int applied_pairs = 0;
	int applied_elements = 0;
	int skipped_warning = 0;
	int skipped_error = 0;
	int skipped_non_candidate = 0;
};

class ContinuationPairIdAssignmentService
{
	public:
		ContinuationPairIdAssignmentResult assignSuggestedPairIds(QETProject &project) const;
};

#endif // CONTINUATIONPAIRIDASSIGNMENTSERVICE_H
