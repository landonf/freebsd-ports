# $FreeBSD$

PORTNAME=	appmenu-gtk-module
PORTVERSION=	0.7.2
CATEGORIES=	devel
MASTER_SITES=	https://gitlab.com/vala-panel-project/vala-panel-appmenu/uploads/${GL_HASH}/
PKGNAMESUFFIX=	-${FLAVOR}
DISTNAME=	vala-panel-appmenu-${PORTVERSION}

MAINTAINER=	landonf@freebsd.org
.if ${FLAVOR:Umeta} == meta
COMMENT=	GTK+ modules to display old-style menus in the global menu bar
.elif ${FLAVOR:U} == common
COMMENT=	GTK+ module to display old-style menus in the global menu bar (common files)
.elif ${FLAVOR:U} == gtk2
COMMENT=	GTK+ 2.x module to display old-style menus in the global menu bar
.elif ${FLAVOR:U} == gtk3
COMMENT=	GTK+ 3.x module to display old-style menus in the global menu bar
.endif

LICENSE=	LGPL3
.if ${FLAVOR:Umeta} != meta
LICENSE_FILE=	${WRKSRC}/LICENSE
.endif

meta_RUN_DEPENDS=	${PORTNAME}-gtk2>=${PORTVERSION}:devel/${PORTNAME}@gtk2 \
			${PORTNAME}-gtk3>=${PORTVERSION}:devel/${PORTNAME}@gtk3
gtk2_RUN_DEPENDS=	${PORTNAME}-common>=${PORTVERSION}:devel/${PORTNAME}@common
gtk3_RUN_DEPENDS=	${PORTNAME}-common>=${PORTVERSION}:devel/${PORTNAME}@common

FLAVORS=	meta gtk3 gtk2 common
FLAVOR?=	${FLAVORS:[1]}
meta_PKGNAMESUFFIX=

.if ${FLAVOR:U} == meta
USES=		metaport
.else
USES=		cmake gnome tar:xz
.endif

.if ${FLAVOR:U} == common
USE_GNOME=	glib20
CMAKE_ON=	ENABLE_APPMENU_COMMON
GLIB_SCHEMAS=	org.appmenu.gtk-module.gschema.xml
.elif ${FLAVOR:U} == gtk2
USE_GNOME=	gtk20
CMAKE_ON=	ENABLE_APPMENU_GTK2_MODULE
.elif ${FLAVOR:U} == gtk3
USE_GNOME=	gtk30
CMAKE_ON=	ENABLE_APPMENU_GTK3_MODULE
.endif

PLIST=		${PKGDIR}/pkg-plist.${FLAVOR}
meta_PLIST=

.if ${FLAVOR:U} == meta
PATCHDIR=	/dev/null
.endif

GL_HASH=	57fe7a74b38180002154fbd6028075db

# Extract only the appmenu-gtk-module subproject
EXTRACT_AFTER_ARGS=	\
	--no-same-owner --no-same-permissions \
	--include "^${DISTNAME}/subprojects/${PORTNAME}" \
	-s "|^${DISTNAME}/subprojects/${PORTNAME}/\(.*\)|${DISTNAME}/\1|"

CMAKE_OFF+=	\
	GSETTINGS_LOCALINSTALL \
	GSETTINGS_COMPILE

.include <bsd.port.mk>
