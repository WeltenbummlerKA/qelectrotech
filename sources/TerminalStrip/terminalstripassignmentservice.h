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
#ifndef TERMINALSTRIPASSIGNMENTSERVICE_H
#define TERMINALSTRIPASSIGNMENTSERVICE_H

#include <QString>

class QETProject;
class TerminalStrip;

class TerminalStripAssignmentService
{
	public:
		struct Result
		{
			TerminalStrip *strip = nullptr;
			bool created_strip = false;
			int matched_free_terminals = 0;
			int added_terminals = 0;
			int matching_strip_terminals = 0;
			bool reordered = false;
			bool skipped_non_single_level_strip = false;
		};

		static Result assignSingleLevelByPrefix(QETProject *project, const QString &prefix);
};

#endif // TERMINALSTRIPASSIGNMENTSERVICE_H
