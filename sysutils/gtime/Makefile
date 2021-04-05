# $FreeBSD$

PORTNAME=	time
PORTVERSION=	1.9
CATEGORIES=	sysutils textproc
MASTER_SITES=	GNU
PKGNAMEPREFIX=	g

MAINTAINER=	landonf@FreeBSD.org
COMMENT=	Free Software Foundation "time" utility

LICENSE=	GPLv3
LICENSE_FILE=	${WRKSRC}/COPYING

USES=		gmake
TEST_TARGET=	check

GNU_CONFIGURE=	yes
CONFIGURE_ARGS=	--program-prefix=g

INFO=		time

.include <bsd.port.mk>
