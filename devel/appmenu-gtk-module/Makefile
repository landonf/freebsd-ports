# $FreeBSD$

PORTNAME=	appmenu-gtk-module
PORTVERSION=	0.7.2
CATEGORIES=	devel
MASTER_SITES=	https://gitlab.com/vala-panel-project/vala-panel-appmenu/uploads/${GL_HASH}/
DISTNAME=	vala-panel-appmenu-${PORTVERSION}

MAINTAINER=	landonf@freebsd.org
COMMENT=	GTK+ modules to present application menus within the global menu bar

LICENSE=	LGPL3
LICENSE_FILE=	${WRKSRC}/LICENSE

USES=		cmake pkgconfig gnome xorg tar:xz
USE_GNOME=	glib20
USE_XORG=	x11

GL_HASH=	57fe7a74b38180002154fbd6028075db

CMAKE_ON=	ENABLE_APPMENU_COMMON
CMAKE_OFF=	GSETTINGS_LOCALINSTALL \
		GSETTINGS_COMPILE

GLIB_SCHEMAS=	org.appmenu.gtk-module.gschema.xml

OPTIONS_MULTI=	GTK
OPTIONS_MULTI_GTK=	GTK2 GTK3
OPTIONS_DEFAULT=	GTK2 GTK3
OPTIONS_SUB=	yes
GTK2_DESC=	Install GTK+ 2.x module
GTK3_DESC=	Install GTK+ 3.x module

GTK2_CMAKE_BOOL=	ENABLE_APPMENU_GTK2_MODULE
GTK2_VARS=	USE_GNOME+=gtk20

GTK3_CMAKE_BOOL=	ENABLE_APPMENU_GTK3_MODULE
GTK3_VARS=	USE_GNOME+=gtk30

# Extract only the appmenu-gtk-module subproject
EXTRACT_AFTER_ARGS=	\
	--no-same-owner --no-same-permissions \
	--include "^${DISTNAME}/subprojects/${PORTNAME}" \
	-s "|^${DISTNAME}/subprojects/${PORTNAME}/\(.*\)|${DISTNAME}/\1|"

.include <bsd.port.mk>
