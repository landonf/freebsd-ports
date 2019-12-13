# $FreeBSD$

PORTNAME=	diskmonitor
#DISTVERSIONPREFIX=	v
DISTVERSION=	0.3.3
CATEGORIES=	sysutils kde

MAINTAINER=	landonf@FreeBSD.org
COMMENT=	Plasma application and applet for monitoring SMART device status

LICENSE=	GPLv2+
LICENSE_FILE=	${WRKSRC}/LICENSE

RUN_DEPENDS=	${LOCALBASE}/bin/bsdisks:sysutils/bsdisks

USES=		cmake compiler:c++11-lib kde:5 qt:5
USE_KDE=	auth codecs config configwidgets coreaddons decoration i18n iconthemes \
		notifications plasma-framework widgetsaddons windowsystem xmlgui \
		ecm_build
USE_QT=		core dbus declarative gui network widgets xml \
		buildtools_build qmake_build

USE_GITHUB=	yes
GH_ACCOUNT=	papylhomme
GH_PROJECT=	diskmonitor

.include <bsd.port.mk>
