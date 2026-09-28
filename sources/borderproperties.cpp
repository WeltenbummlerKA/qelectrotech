/*
	Copyright 2006-2026 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "borderproperties.h"

/**
	@brief BorderProperties::BorderProperties
	constructor
	Initializes a BorderProperties object with default properties

	Initializes a BorderProperties object
	with the following default properties:
	- 17 columns of 60.0 px wide by 20.0px high
	- 8    lines of 80.0 px high by 20.0px wide

	\~French Initialise un objet BorderProperties avec les proprietes par
	defaut suivantes :
	- 17 colonnes affichees de 60.0 px de large pour 20.0px de haut
	- 8    lignes affichees de 80.0 px de haut pour 20.0px de large
*/
BorderProperties::BorderProperties() :
	columns_count(17),
	columns_width(60.0),
	columns_header_height(20.0),
	bottom_columns_header_height(0.0),
	heading_height(0.0),
	display_columns(true),
	columns_start_at_zero(QSettings().value("border-columns_0", true).toBool()),
	rows_count(8),
	rows_height(80.0),
	rows_header_width(20.0),
	display_rows(true),
	row_headers_both_sides(false),
	outer_border_margin(0.0)
{
}

/**
	@brief BorderProperties::~BorderProperties
	destructor
*/
BorderProperties::~BorderProperties()
{
}

/**
	@brief BorderProperties::operator ==

	\~ @param bp : Other BorderProperties container/class.
	\~French Autre conteneur BorderProperties
	\~ @return True if it and this container are identical, false otherwise.
	\~French True si ip et ce conteneur sont identiques, false sinon
*/
bool BorderProperties::operator==(const BorderProperties &bp) {
	return(
		bp.columns_count == columns_count &&\
		bp.columns_width == columns_width &&\
		bp.columns_header_height == columns_header_height &&\
		bp.bottom_columns_header_height == bottom_columns_header_height &&\
		bp.heading_height == heading_height &&\
		bp.display_columns == display_columns &&\
		bp.columns_start_at_zero == columns_start_at_zero &&\
		bp.rows_count == rows_count &&\
		bp.rows_height == rows_height &&\
		bp.rows_header_width == rows_header_width &&\
		bp.display_rows == display_rows &&\
		bp.row_headers_both_sides == row_headers_both_sides &&\
		bp.outer_border_margin == outer_border_margin
	);
}

/**
	@brief BorderProperties::operator !=

	\~ @param bp :
	Other BorderProperties container/class.
	\~French Autre conteneur BorderProperties
	\~ @return
	False if it and this container are identical, true otherwise.
	\~French False si bp et ce conteneur sont identiques, true sinon
*/
bool BorderProperties::operator!=(const BorderProperties &bp) {
	return(!(*this == bp));
}

/**
	@brief BorderProperties::toXml
	Exports dimensions as XML attributes added to element e.
	\~French Exporte les dimensions sous formes d'attributs XML ajoutes a l'element e.

	\~ @param e :
	XML element to which attributes will be added
	\~French Element XML auquel seront ajoutes des attributs
*/
void BorderProperties::toXml(QDomElement &e) const
{
	e.setAttribute("cols",        columns_count);
	e.setAttribute("colsize",     QString("%1").arg(columns_width));
	e.setAttribute("colheaderheight", QString::number(columns_header_height));
	e.setAttribute("bottomcolheaderheight", QString::number(bottom_columns_header_height));
	e.setAttribute("headingheight", QString::number(heading_height));
	e.setAttribute("rows",        rows_count);
	e.setAttribute("rowsize",     QString("%1").arg(rows_height));
	e.setAttribute("rowheaderwidth", QString::number(rows_header_width));
	e.setAttribute("displaycols", display_columns ? "true" : "false");
	e.setAttribute("columnstartatzero", columns_start_at_zero ? "true" : "false");
	e.setAttribute("displayrows", display_rows    ? "true" : "false");
	e.setAttribute("rowheadersbothsides", row_headers_both_sides ? "true" : "false");
	e.setAttribute("outerbordermargin", QString::number(outer_border_margin));
}

/**
	@brief BorderProperties::fromXml
	Import dimensions from XML attributes of element e
	\~French Importe les dimensions a partir des attributs XML de l'element e

	\~ @param e :
	XML element whose attributes will be read
	\~French Element XML dont les attributs seront lus
*/
void BorderProperties::fromXml(QDomElement &e) {
	if (e.hasAttribute("cols"))        columns_count   = e.attribute("cols").toInt();
	if (e.hasAttribute("colsize"))     columns_width   = e.attribute("colsize").toDouble();
	if (e.hasAttribute("colheaderheight")) columns_header_height = e.attribute("colheaderheight").toDouble();
	if (e.hasAttribute("bottomcolheaderheight")) bottom_columns_header_height = e.attribute("bottomcolheaderheight").toDouble();
	if (e.hasAttribute("headingheight")) heading_height = e.attribute("headingheight").toDouble();
	if (e.hasAttribute("rows"))        rows_count      = e.attribute("rows").toInt();
	if (e.hasAttribute("rowsize"))     rows_height     = e.attribute("rowsize").toDouble();
	if (e.hasAttribute("rowheaderwidth")) rows_header_width = e.attribute("rowheaderwidth").toDouble();
	if (e.hasAttribute("displaycols")) display_columns = e.attribute("displaycols") == "true";
	if (e.hasAttribute("columnstartatzero")) columns_start_at_zero = e.attribute("columnstartatzero") == "true";
	if (e.hasAttribute("displayrows")) display_rows    = e.attribute("displayrows") == "true";
	if (e.hasAttribute("rowheadersbothsides")) row_headers_both_sides = e.attribute("rowheadersbothsides") == "true";
	if (e.hasAttribute("outerbordermargin")) outer_border_margin = e.attribute("outerbordermargin").toDouble();
}

/**
	@brief BorderProperties::toSettings
	Export dimensions in a QSettings object.
	\~French Exporte les dimensions dans une configuration.

	\~ @param settings :
	QSettings object to write
	\~French Parametres a ecrire
	\~ @param prefix :
	prefix to be added before the names of the parameters
	\~French prefixe a ajouter devant les noms des parametres
*/
void BorderProperties::toSettings(QSettings &settings, const QString &prefix) const
{
	settings.setValue(prefix % "cols",        columns_count);
	settings.setValue(prefix % "colsize",     columns_width);
	settings.setValue(prefix % "colheaderheight", columns_header_height);
	settings.setValue(prefix % "bottomcolheaderheight", bottom_columns_header_height);
	settings.setValue(prefix % "headingheight", heading_height);
	settings.setValue(prefix % "displaycols", display_columns);
	settings.setValue(prefix % "columnstartatzero", columns_start_at_zero);
	settings.setValue(prefix % "rows",        rows_count);
	settings.setValue(prefix % "rowsize",     rows_height);
	settings.setValue(prefix % "rowheaderwidth", rows_header_width);
	settings.setValue(prefix % "displayrows", display_rows);
	settings.setValue(prefix % "rowheadersbothsides", row_headers_both_sides);
	settings.setValue(prefix % "outerbordermargin", outer_border_margin);
}

/**
	@brief BorderProperties::fromSettings
	Import dimensions from a QSettings object.
	\~French Importe les dimensions depuis une configuration.
	\~ @param settings : QSettings object to read
	\~French Parametres a lire
	\~ @param prefix : prefix to be added before the names of the parameters
	\~French prefixe a ajouter devant les noms des parametres
*/
void BorderProperties::fromSettings(QSettings &settings, const QString &prefix) {
	columns_count   = settings.value(prefix % "cols",            columns_count).toInt();
	columns_width   = settings.value(prefix % "colsize",  columns_width).toDouble();
	columns_header_height = settings.value(prefix % "colheaderheight", columns_header_height).toDouble();
	bottom_columns_header_height = settings.value(prefix % "bottomcolheaderheight", bottom_columns_header_height).toDouble();
	heading_height = settings.value(prefix % "headingheight", heading_height).toDouble();
	display_columns = settings.value(prefix % "displaycols",     display_columns).toBool();
	columns_start_at_zero = settings.value(prefix % "columnstartatzero", columns_start_at_zero).toBool();
	
	rows_count      = settings.value(prefix % "rows",            rows_count).toInt();
	rows_height     = settings.value(prefix % "rowsize",  rows_height).toDouble();
	rows_header_width = settings.value(prefix % "rowheaderwidth", rows_header_width).toDouble();
	display_rows    = settings.value(prefix % "displayrows",     display_rows).toBool();
	row_headers_both_sides = settings.value(prefix % "rowheadersbothsides", row_headers_both_sides).toBool();
	outer_border_margin = settings.value(prefix % "outerbordermargin", outer_border_margin).toDouble();
}

/**
	@brief BorderProperties::defaultProperties
	@return the default properties stored in the setting file
*/
BorderProperties BorderProperties::defaultProperties()
{
	QSettings settings;

	BorderProperties def;
	def.fromSettings(settings, "diagrameditor/default");

	return(def);
}
