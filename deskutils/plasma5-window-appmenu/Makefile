# $FreeBSD$

PORTNAME=	plasma5-window-appmenu
DISTVERSIONPREFIX=	v
DISTVERSION=	0.6.0
CATEGORIES=	deskutils kde

MAINTAINER=	landonf@FreeBSD.org
COMMENT=	Plasma applet that displays the current window appmenu

LICENSE=	GPLv2+
LICENSE_FILE=	${WRKSRC}/LICENSE

USES=		cmake compiler:c++11-lib kde:5 qt:5 xorg
USE_KDE=	auth codecs config configwidgets coreaddons decoration \
		package plasma-framework service widgetsaddons windowsystem \
		ecm_build
USE_QT=		core dbus declarative gui multimedia network \
		opengl printsupport widgets xml x11extras \
		buildtools_build qmake_build
USE_XORG=	xcb xrandr

USE_GITHUB=	yes
GH_ACCOUNT=	psifidotos
GH_PROJECT=	applet-window-appmenu

.include <bsd.port.mk>
