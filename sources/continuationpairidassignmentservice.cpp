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
#include "continuationpairidassignmentservice.h"

#include "continuationprojectionservice.h"
#include "diagram.h"
#include "diagramcontext.h"
#include "qetgraphicsitem/element.h"
#include "qetproject.h"

#include <QHash>

namespace {

QString uuidKey(const QUuid &uuid)
{
	return uuid.toString(QUuid::WithoutBraces);
}

bool isSafePairIdCandidate(const ContinuationProjection &projection)
{
	return projection.migration_recommendation == QStringLiteral("candidate: assign mam_pair_id")
		&& !projection.suggested_mam_pair_id.isEmpty()
		&& projection.mam_pair_id.isEmpty()
		&& projection.chain.isEmpty()
		&& projection.mam_chain_id.isEmpty()
		&& projection.chain_order < 0
		&& projection.mam_chain_order < 0;
}

QHash<QString, Element *> elementsByUuid(QETProject &project)
{
	QHash<QString, Element *> elements;
	for (Diagram *diagram : project.diagrams()) {
		if (!diagram) {
			continue;
		}
		for (Element *element : diagram->elements()) {
			if (element) {
				elements.insert(uuidKey(element->uuid()), element);
			}
		}
	}
	return elements;
}

} // namespace

ContinuationPairIdAssignmentResult ContinuationPairIdAssignmentService::assignSuggestedPairIds(QETProject &project) const
{
	ContinuationPairIdAssignmentResult result;
	const QList<ContinuationProjection> projections =
		ContinuationProjectionService().continuations(project);

	QHash<QString, QList<ContinuationProjection>> candidates_by_pair_id;
	for (const ContinuationProjection &projection : projections) {
		if (isSafePairIdCandidate(projection)) {
			candidates_by_pair_id[projection.suggested_mam_pair_id] << projection;
		} else if (projection.status == QStringLiteral("ERROR")) {
			++result.skipped_error;
		} else if (projection.status == QStringLiteral("WARNING")) {
			++result.skipped_warning;
		} else {
			++result.skipped_non_candidate;
		}
	}

	const QHash<QString, Element *> elements = elementsByUuid(project);
	for (auto it = candidates_by_pair_id.constBegin(); it != candidates_by_pair_id.constEnd(); ++it) {
		const QList<ContinuationProjection> pair = it.value();
		if (pair.size() != 2) {
			result.skipped_non_candidate += pair.size();
			continue;
		}

		QList<Element *> pair_elements;
		bool missing_element = false;
		for (const ContinuationProjection &projection : pair) {
			Element *element = elements.value(uuidKey(projection.uuid), nullptr);
			if (!element) {
				missing_element = true;
				break;
			}
			pair_elements << element;
		}
		if (missing_element) {
			result.skipped_non_candidate += pair.size();
			continue;
		}

		for (Element *element : pair_elements) {
			DiagramContext info = element->elementInformations();
			info.addValue(QStringLiteral("mam_pair_id"), it.key(), false);
			element->setElementInformations(info);
			++result.applied_elements;
		}
		++result.applied_pairs;
	}

	return result;
}
