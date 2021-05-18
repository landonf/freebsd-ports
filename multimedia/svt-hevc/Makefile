PORTNAME=	svt-hevc
DISTVERSIONPREFIX=	v
DISTVERSION=	1.5.1
CATEGORIES=	multimedia

MAINTAINER=	jbeich@FreeBSD.org
COMMENT=	Scalable HEVC encoder

LICENSE=	BSD2CLAUSE+PATENT
LICENSE_GROUPS=	FSF OSI COPYFREE
LICENSE_NAME=	BSD-2-Clause Plus Patent License
LICENSE_FILE=	${WRKSRC}/LICENSE.md
LICENSE_PERMS=	dist-mirror dist-sell pkg-mirror pkg-sell auto-accept

ONLY_FOR_ARCHS=	amd64
ONLY_FOR_ARCHS_REASON=	uses x86-64 assembly

BUILD_DEPENDS=	nasm:devel/nasm

USES=		cmake compiler:c11
USE_GITHUB=	yes
USE_LDCONFIG=	yes
GH_ACCOUNT=	OpenVisualCloud
GH_PROJECT=	SVT-HEVC

OPTIONS_DEFINE=	NATIVE
OPTIONS_RADIO=	PROF
OPTIONS_RADIO_PROF=	PROFILE PGO
OPTIONS_DEFAULT=	PGO

NATIVE_DESC=	Build with native optimizations (-march=native)
PROF_DESC=	Profile-guided Optimization
PROFILE_DESC=	Generate instrumented code
PGO_DESC=	Enable profile-guided optimizations

NATIVE_CMAKE_BOOL=	NATIVE
PROFILE_CFLAGS=	-fprofile-instr-generate=/dev/null
PGO_CFLAGS=	-fprofile-instr-use=${FILESDIR}/svt-hevc.profdata

WITHOUT_NO_STRICT_ALIASING=yes

CFLAGS+=	-O3 -flto
LDFLAGS+=	-flto

OPTIONS_DEFINE=	LTO
OPTIONS_DEFAULT=LTO

LTO_CMAKE_BOOL=		CMAKE_INTERPROCEDURAL_OPTIMIZATION
LTO_CMAKE_ON=		-DCMAKE_POLICY_DEFAULT_CMP0069:STRING=NEW
.if exists(/usr/bin/ld.lld) && ${/usr/bin/ld:L:tA} != /usr/bin/ld.lld
# --plugin isn't supported by old GNU ld.bfd in base
LTO_LDFLAGS=		-fuse-ld=lld
.endif

post-patch:
# Keep using nasm and drop vendor -O/-g
	@${REINPLACE_CMD} -e '/find_program.*yasm/d' \
		-Ee '/set.*(release|debug)_flags_to_test/,/\)/d' \
		${WRKSRC}/CMakeLists.txt

.include <bsd.port.mk>
