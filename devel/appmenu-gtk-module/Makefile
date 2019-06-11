# $FreeBSD$

PORTNAME=	appmenu-gtk-module
PORTVERSION=	0.7.2
CATEGORIES=	devel
DISTNAME=	vala-panel-appmenu-${PORTVERSION}
MASTER_SITES=	https://gitlab.com/vala-panel-project/vala-panel-appmenu/uploads/${GL_HASH}/
GL_HASH=	57fe7a74b38180002154fbd6028075db

MAINTAINER=	landonf@freebsd.org
COMMENT=	GTK+ module to present application menus within the global menu bar

LICENSE=	LGPL3

RUN_DEPENDS=	${PORTNAME}-gtk2>=${PORTVERSION}:devel/${PORTNAME}-gtk2 \
		${PORTNAME}-gtk3>=${PORTVERSION}:devel/${PORTNAME}-gtk3

.if !defined(SLAVE_PORT)
USES=		metaport
PATCHDIR=	/dev/null
.else
LICENSE_FILE=	${WRKSRC}/LICENSE
RUN_DEPENDS=	${PORTNAME}-common>=${PORTVERSION}:devel/${PORTNAME}-common
USES=		cmake gnome tar:xz
PLIST=		${PKGDIR}/pkg-plist.${SLAVE_PORT}

# Extract only the appmenu-gtk-module subproject
EXTRACT_AFTER_ARGS+=	\
	--include "^${DISTNAME}/subprojects/${PORTNAME}" \
	-s "|^${DISTNAME}/subprojects/${PORTNAME}/\(.*\)|${DISTNAME}/\1|"

CMAKE_OFF+=	\
	GSETTINGS_LOCALINSTALL \
	GSETTINGS_COMPILE

.if ${SLAVE_PORT} == "common"
COMMENT=	GTK+ module to present application menus within the global menu bar (common files)
RUN_DEPENDS=	
USE_GNOME=	glib20
CMAKE_ON=	ENABLE_APPMENU_COMMON
GLIB_SCHEMAS=	org.appmenu.gtk-module.gschema.xml
.elif ${SLAVE_PORT} == "gtk2"
COMMENT=	GTK+ 2.x module to present application menus within the global menu bar
USE_GNOME=	gtk20
CMAKE_ON=	ENABLE_APPMENU_GTK2_MODULE
.elif ${SLAVE_PORT} == "gtk3"
COMMENT=	GTK+ 3.x module to present application menus within the global menu bar
USE_GNOME=	gtk30
CMAKE_ON=	ENABLE_APPMENU_GTK3_MODULE
.endif

.endif


.include <bsd.port.mk>
