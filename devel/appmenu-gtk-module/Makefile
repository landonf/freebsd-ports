# $FreeBSD$

PORTNAME=	appmenu-gtk-module
PORTVERSION=	0.7.2
CATEGORIES=	devel
MASTER_SITES=	https://gitlab.com/vala-panel-project/vala-panel-appmenu/uploads/${GL_HASH}/
DISTNAME=	vala-panel-appmenu-${PORTVERSION}

MAINTAINER=	landonf@freebsd.org
COMMENT=	${COMMENT_${FLAVOR:Umeta}}
COMMENT_meta=	GTK+ modules to display old-style menus in the global menu bar
COMMENT_common=	GTK+ module to display old-style menus in the global menu bar (common files)
COMMENT_gtk2=	GTK+ 2.x module to display old-style menus in the global menu bar
COMMENT_gtk3=	GTK+ 3.x module to display old-style menus in the global menu bar

.if ${FLAVOR:Umeta} != meta
LICENSE=	LGPL3
LICENSE_FILE=	${WRKSRC}/LICENSE
.endif

meta_RUN_DEPENDS=	${PORTNAME}-gtk2>=${PORTVERSION}:devel/${PORTNAME}@gtk2 \
			${PORTNAME}-gtk3>=${PORTVERSION}:devel/${PORTNAME}@gtk3
gtk2_RUN_DEPENDS=	${PORTNAME}-common>=${PORTVERSION}:devel/${PORTNAME}@common
gtk3_RUN_DEPENDS=	${PORTNAME}-common>=${PORTVERSION}:devel/${PORTNAME}@common

FLAVORS=	meta gtk3 gtk2 common
FLAVOR?=	${FLAVORS:[1]}
gtk3_PKGNAMESUFFIX=	-gtk3
gtk2_PKGNAMESUFFIX=	-gtk2
common_PKGNAMESUFFIX=	-common

.if ${FLAVOR} == meta
USES=		metaport
.else
USES=		cmake gnome tar:xz
.endif

.if ${FLAVOR} == common
USE_GNOME=	glib20
CMAKE_ON=	ENABLE_APPMENU_COMMON
GLIB_SCHEMAS=	org.appmenu.gtk-module.gschema.xml
.elif ${FLAVOR} == gtk2
USE_GNOME=	gtk20
CMAKE_ON=	ENABLE_APPMENU_GTK2_MODULE
.elif ${FLAVOR} == gtk3
USE_GNOME=	gtk30
CMAKE_ON=	ENABLE_APPMENU_GTK3_MODULE
.endif

PLIST=		${PKGDIR}/pkg-plist
gtk3_PLIST=	${PKGDIR}/pkg-plist.gtk3
gtk2_PLIST=	${PKGDIR}/pkg-plist.gtk2
common_PLIST=	${PKGDIR}/pkg-plist.common

.if ${FLAVOR} == meta
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
