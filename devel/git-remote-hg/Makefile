# $FreeBSD$

PORTNAME=	git-remote-hg
PORTVERSION=	1.0.2.1
DISTVERSIONPREFIX=	v
CATEGORIES=	devel python

MAINTAINER=	landonf@FreeBSD.org
COMMENT=	Transparent bidirectional bridge between Git and Mercurial for Git

LICENSE=	GPLv2
LICENSE_FILE=	${WRKSRC}/LICENSE

BUILD_DEPENDS=	a2x:textproc/asciidoc
RUN_DEPENDS=	${PY_MERCURIAL}

USES=		gmake python:3.8+ shebangfix

USE_GITHUB=	yes
GH_ACCOUNT=	mnauw
MAKE_ENV=	prefix="${PREFIX}"
SHEBANG_FILES=	git-remote-hg

INSTALL_TARGET=	install install-doc

PLIST_FILES=	bin/git-remote-hg man/man1/git-remote-hg.1.gz

.include <bsd.port.mk>
