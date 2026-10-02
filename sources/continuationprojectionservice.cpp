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
#include "continuationprojectionservice.h"

#include "diagram.h"
#include "diagramcontext.h"
#include "diagramposition.h"
#include "qetgraphicsitem/element.h"
#include "qetproject.h"

#include <QHash>
#include <QRegularExpression>

#include <algorithm>

namespace {

QString elementLocationKey(Element *element)
{
	if (!element) {
		return QString();
	}

	const ElementsLocation location = element->location();
	const QString collection_path = location.collectionPath(true);
	return collection_path.isEmpty() ? location.path() : collection_path;
}

bool isMamContinuationElement(Element *element)
{
	const QString location = elementLocationKey(element);
	return location.contains(QStringLiteral("mam_potential_"));
}

QString continuationDirection(Element *element)
{
	const QString location = elementLocationKey(element);
	if (location.contains(QStringLiteral("previous_h"))) {
		return QStringLiteral("left");
	}
	if (location.contains(QStringLiteral("next_down"))) {
		return QStringLiteral("down");
	}
	if (location.contains(QStringLiteral("next_h"))) {
		return QStringLiteral("right");
	}
	return QStringLiteral("unknown");
}

QString gridPath(const QString &grid)
{
	static const QRegularExpression re(QStringLiteral("(\\d+)$"));
	const QRegularExpressionMatch match = re.match(grid);
	return match.hasMatch() ? match.captured(1) : QString();
}

QString xrefText(int folio, const QString &path)
{
	if (folio < 0 || path.isEmpty()) {
		return QString();
	}
	return QStringLiteral("/%1.%2").arg(folio).arg(path);
}

QString groupKey(const ContinuationProjection &projection)
{
	if (!projection.mam_pair_id.isEmpty()) {
		return QStringLiteral("mam_pair_id:") % projection.mam_pair_id;
	}
	if (!projection.chain.isEmpty()) {
		return QStringLiteral("chain:") % projection.chain;
	}
	if (!projection.signal.isEmpty()) {
		return QStringLiteral("signal:") % projection.signal;
	}
	if (!projection.potential.isEmpty()) {
		return QStringLiteral("potential:") % projection.potential;
	}
	return QStringLiteral("uuid:") % projection.uuid.toString(QUuid::WithoutBraces);
}

QString statusFor(const QStringList &diagnostics)
{
	for (const QString &diagnostic : diagnostics) {
		if (diagnostic.startsWith(QStringLiteral("ERROR:"))) {
			return QStringLiteral("ERROR");
		}
	}
	for (const QString &diagnostic : diagnostics) {
		if (diagnostic.startsWith(QStringLiteral("WARNING:"))) {
			return QStringLiteral("WARNING");
		}
	}
	return QStringLiteral("OK");
}

bool isForwardDirection(const QString &direction)
{
	return direction == QStringLiteral("right")
		|| direction == QStringLiteral("down");
}

bool isBackwardDirection(const QString &direction)
{
	return direction == QStringLiteral("left")
		|| direction == QStringLiteral("up");
}

bool hasOppositeDirection(const ContinuationProjection &left, const ContinuationProjection &right)
{
	return (isForwardDirection(left.direction) && isBackwardDirection(right.direction))
		|| (isBackwardDirection(left.direction) && isForwardDirection(right.direction));
}

void setTarget(ContinuationProjection &source, const ContinuationProjection &target)
{
	source.target_uuid = target.uuid;
	source.target_folio = target.folio;
	source.target_grid = target.grid;
	source.target_path = target.path;
	source.computed_xref = xrefText(target.folio, target.path);
}

void checkVisibleText(ContinuationProjection &projection)
{
	if (projection.computed_xref.isEmpty()) {
		return;
	}
	if (projection.visible_xref.isEmpty()) {
		projection.diagnostics << QStringLiteral("WARNING: empty visible xref");
		return;
	}
	if (projection.visible_xref != projection.computed_xref) {
		projection.diagnostics << QStringLiteral("WARNING: stale visible xref %1, expected %2")
			.arg(projection.visible_xref, projection.computed_xref);
	}
}

bool hasExplicitChain(const ContinuationProjection &projection)
{
	return !projection.mam_chain_id.isEmpty();
}

bool hasAnyChain(const ContinuationProjection &projection)
{
	return !projection.chain.isEmpty();
}

QString chainRelationship(const ContinuationProjection &projection)
{
	if (!projection.mam_chain_id.isEmpty()) {
		return QStringLiteral("mam_chain %1 order %2")
			.arg(projection.mam_chain_id)
			.arg(projection.chain_order);
	}
	return QStringLiteral("chain %1 order %2")
		.arg(projection.chain)
		.arg(projection.chain_order);
}

} // namespace

QList<ContinuationProjection> ContinuationProjectionService::continuations(QETProject &project) const
{
	QList<ContinuationProjection> projections;

	QHash<Diagram *, int> folios;
	int folio = 0;
	for (Diagram *diagram : project.diagrams()) {
		folios.insert(diagram, ++folio);
	}

	for (Diagram *diagram : project.diagrams()) {
		if (!diagram) {
			continue;
		}

		for (Element *element : diagram->elements()) {
			if (!element || !isMamContinuationElement(element)) {
				continue;
			}

			const DiagramContext info = element->elementInformations();
			const QString grid = diagram->convertPosition(element->scenePos()).toString();

			ContinuationProjection projection;
			projection.uuid = element->uuid();
			projection.potential = info.value(QStringLiteral("potential")).toString().trimmed();
			projection.signal = info.value(QStringLiteral("signal")).toString().trimmed();
			projection.voltage = info.value(QStringLiteral("voltage")).toString().trimmed();
			projection.mam_continuation_id = info.value(QStringLiteral("mam_continuation_id")).toString().trimmed();
			projection.mam_pair_id = info.value(QStringLiteral("mam_pair_id")).toString().trimmed();
			projection.mam_chain_id = info.value(QStringLiteral("mam_chain_id")).toString().trimmed();
			bool mam_order_ok = false;
			const int mam_order = info.value(QStringLiteral("mam_chain_order")).toString().toInt(&mam_order_ok);
			projection.mam_chain_order = mam_order_ok ? mam_order : -1;
			const QString legacy_chain = info.value(QStringLiteral("chain")).toString().trimmed();
			bool legacy_order_ok = false;
			const int legacy_order = info.value(QStringLiteral("chain_order")).toString().toInt(&legacy_order_ok);
			projection.chain = projection.mam_chain_id.isEmpty() ? legacy_chain : projection.mam_chain_id;
			projection.chain_order = mam_order_ok ? mam_order : (legacy_order_ok ? legacy_order : -1);
			projection.direction = continuationDirection(element);
			projection.folio = folios.value(diagram, -1);
			projection.grid = grid;
			projection.path = gridPath(grid);
			projection.visible_xref = info.value(QStringLiteral("xref")).toString().trimmed();
			projection.cardinality = QStringLiteral("unresolved");
			projection.relationship = projection.chain.isEmpty()
				? QStringLiteral("auto-by-potential")
				: QStringLiteral("chain %1").arg(projection.chain);

			if (projection.potential.isEmpty() && projection.signal.isEmpty()) {
				projection.diagnostics << QStringLiteral("ERROR: empty potential or signal");
			}
			if (projection.direction == QStringLiteral("unknown")) {
				projection.diagnostics << QStringLiteral("ERROR: unknown continuation direction");
			}
			if (projection.path.isEmpty()) {
				projection.diagnostics << QStringLiteral("ERROR: empty current path");
			}
			if (!projection.mam_pair_id.isEmpty() && hasAnyChain(projection)) {
				projection.diagnostics << QStringLiteral("ERROR: mam_pair_id cannot be combined with mam_chain_id or legacy chain");
			}

			projections << projection;
		}
	}

	std::sort(projections.begin(), projections.end(), [](const ContinuationProjection &left, const ContinuationProjection &right) {
		const QString left_key = QStringLiteral("%1|%2|%3|%4")
			.arg(left.folio)
			.arg(left.grid)
			.arg(left.potential)
			.arg(left.uuid.toString(QUuid::WithoutBraces));
		const QString right_key = QStringLiteral("%1|%2|%3|%4")
			.arg(right.folio)
			.arg(right.grid)
			.arg(right.potential)
			.arg(right.uuid.toString(QUuid::WithoutBraces));
		return left_key < right_key;
	});

	QHash<QString, QList<int>> groups;
	for (int i = 0; i < projections.size(); ++i) {
		groups[groupKey(projections.at(i))] << i;
	}

	for (const QList<int> &indexes : groups) {
		if (indexes.size() == 1) {
			ContinuationProjection &projection = projections[indexes.first()];
			if (!projection.mam_pair_id.isEmpty()) {
				projection.diagnostics << QStringLiteral("ERROR: mam_pair_id expects exactly two continuations");
			} else if (!projection.potential.isEmpty() || !projection.signal.isEmpty()) {
				projection.diagnostics << QStringLiteral("ERROR: missing continuation counterpart");
			}
			projection.status = statusFor(projection.diagnostics);
			continue;
		}

		const bool pair_group = std::all_of(indexes.begin(), indexes.end(), [&](int index) {
			return !projections.at(index).mam_pair_id.isEmpty();
		});
		if (pair_group && indexes.size() != 2) {
			for (int index : indexes) {
				ContinuationProjection &projection = projections[index];
				projection.cardinality = QStringLiteral("invalid-pair");
				projection.relationship = QStringLiteral("mam_pair %1").arg(projection.mam_pair_id);
				projection.diagnostics << QStringLiteral("ERROR: mam_pair_id expects exactly two continuations");
				projection.status = statusFor(projection.diagnostics);
			}
			continue;
		}

		if (indexes.size() == 2) {
			ContinuationProjection &first = projections[indexes.at(0)];
			ContinuationProjection &second = projections[indexes.at(1)];
			first.cardinality = QStringLiteral("1:1");
			second.cardinality = QStringLiteral("1:1");
			if (!first.mam_pair_id.isEmpty()) {
				first.relationship = QStringLiteral("mam_pair %1").arg(first.mam_pair_id);
				second.relationship = QStringLiteral("mam_pair %1").arg(second.mam_pair_id);
			} else {
				first.relationship = first.chain.isEmpty()
					? QStringLiteral("auto-point-to-point")
					: chainRelationship(first);
				second.relationship = second.chain.isEmpty()
					? QStringLiteral("auto-point-to-point")
					: chainRelationship(second);
			}
			setTarget(first, second);
			setTarget(second, first);
			if (!hasOppositeDirection(first, second)) {
				first.diagnostics << QStringLiteral("ERROR: contradictory continuation direction");
				second.diagnostics << QStringLiteral("ERROR: contradictory continuation direction");
			}
			checkVisibleText(first);
			checkVisibleText(second);
			first.status = statusFor(first.diagnostics);
			second.status = statusFor(second.diagnostics);
			continue;
		}

		const bool chain_group = std::all_of(indexes.begin(), indexes.end(), [&](int index) {
			return hasAnyChain(projections.at(index));
		});
		if (!chain_group) {
			for (int index : indexes) {
				ContinuationProjection &projection = projections[index];
				projection.cardinality = QStringLiteral("ambiguous");
				projection.diagnostics << QStringLiteral("WARNING: ambiguous continuation group without chain order");
				projection.status = statusFor(projection.diagnostics);
			}
			continue;
		}

		QList<int> ordered = indexes;
		std::sort(ordered.begin(), ordered.end(), [&](int left, int right) {
			return projections.at(left).chain_order < projections.at(right).chain_order;
		});

		QHash<int, int> used_orders;
		for (int index : ordered) {
			ContinuationProjection &projection = projections[index];
			if (projection.chain_order < 0) {
				projection.diagnostics << (hasExplicitChain(projection)
					? QStringLiteral("WARNING: missing mam_chain_order")
					: QStringLiteral("WARNING: missing chain_order"));
			} else if (used_orders.contains(projection.chain_order)) {
				projection.diagnostics << QStringLiteral("WARNING: duplicate chain_order %1").arg(projection.chain_order);
			}
			used_orders.insert(projection.chain_order, index);
		}

		for (int order_index = 0; order_index < ordered.size(); ++order_index) {
			ContinuationProjection &projection = projections[ordered.at(order_index)];
			projection.cardinality = QStringLiteral("chain");
			projection.relationship = chainRelationship(projection);
			int target_index = -1;
			if (isForwardDirection(projection.direction) && order_index + 1 < ordered.size()) {
				target_index = ordered.at(order_index + 1);
			} else if (isBackwardDirection(projection.direction) && order_index > 0) {
				target_index = ordered.at(order_index - 1);
			}
			if (target_index >= 0) {
				setTarget(projection, projections.at(target_index));
				checkVisibleText(projection);
			} else {
				projection.diagnostics << QStringLiteral("ERROR: missing chain target for direction");
			}
			projection.status = statusFor(projection.diagnostics);
		}
	}

	for (ContinuationProjection &projection : projections) {
		if (projection.status.isEmpty()) {
			projection.status = statusFor(projection.diagnostics);
		}
	}

	return projections;
}
