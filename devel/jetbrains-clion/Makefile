PORTNAME=	clion
PORTVERSION=	2021.1.1
CATEGORIES=	devel java
MASTER_SITES=	https://download-cf.jetbrains.com/cpp/
PKGNAMEPREFIX=	jetbrains-
DISTNAME=	CLion-${PORTVERSION}
DIST_SUBDIR=	jetbrains

MAINTAINER=	dmitry.wagin@ya.ru
COMMENT=	JetBrains CLion IDE

LICENSE=	IntelliJ-CLion
LICENSE_NAME=	Subscription license agreement for business and organizations
LICENSE_TEXT=	See: https://www.jetbrains.com/store/license.html
LICENSE_PERMS=	dist-mirror pkg-mirror auto-accept

RUN_DEPENDS=	intellij-fsnotifier>0:java/intellij-fsnotifier \
		intellij-pty4j>=0.11.4:java/intellij-pty4j \
		lsof:sysutils/lsof \
		cmake>=3.14.3:devel/cmake

USES=		linux python:run shebangfix

USE_JAVA=	yes
JAVA_VERSION=	11+

SHEBANG_FILES=	bin/printenv.py bin/restart.py \
		plugins/python-ce/helpers/typeshed/scripts/generate_proto_stubs.sh

NO_ARCH=	yes
NO_BUILD=	yes
SUB_FILES=	${PORTNAME} ${PORTNAME}.desktop pkg-message

WRKSRC=		${WRKDIR}/clion-${PORTVERSION}

CLION_CLANG=	bin/clang/linux

do-install:
# Linux/Windows/OS X only so remove them
	@${RM} -r \
		${WRKSRC}/bin/cmake/linux \
		${WRKSRC}/bin/fsnotifier \
		${WRKSRC}/bin/fsnotifier-arm \
		${WRKSRC}/bin/fsnotifier64 \
		${WRKSRC}/bin/gdb/linux \
		${WRKSRC}/bin/lldb/linux \
		${WRKSRC}/jbr \
		${WRKSRC}/jre64 \
		${WRKSRC}/lib/pty4j-native \
		${WRKSRC}/plugins/cwm-plugin/quiche-native \
		${WRKSRC}/plugins/performanceTesting/bin

# Use cmake binaries provided by devel/cmake
	${MKDIR} ${WRKSRC}/bin/cmake/linux/bin
.for f in ccmake cmake cpack ctest
	${LN} -sf ${PREFIX}/bin/${f} ${WRKSRC}/bin/cmake/linux/bin/${f}
.endfor

	${MKDIR} ${STAGEDIR}${DATADIR}
	@(cd ${WRKSRC} && ${COPYTREE_SHARE} . ${STAGEDIR}${DATADIR} \
		"! -path \./${CLION_CLANG}/* ! -name *\.bak ! -name *\.so ! -name *\.dll ! -name *\.dylib ! -name *\.pdb ! -name *\.sh")
	@(cd ${WRKSRC} && ${COPYTREE_BIN} ${CLION_CLANG} ${STAGEDIR}${DATADIR})
	@(cd ${WRKSRC} && ${COPYTREE_BIN} . ${STAGEDIR}${DATADIR} "-name *\.sh")
	${INSTALL_SCRIPT} ${WRKDIR}/${PORTNAME} ${STAGEDIR}${PREFIX}/bin/${PORTNAME}
	${INSTALL_MAN} ${FILESDIR}/${PORTNAME}.1 ${STAGEDIR}${PREFIX}/man/man1
	${INSTALL_DATA} ${WRKDIR}/${PORTNAME}.desktop ${STAGEDIR}${PREFIX}/share/applications/
# Use fsnotifier replacement provided by java/intellij-fsnotifier
	${ECHO} "idea.filewatcher.executable.path=${PREFIX}/intellij/bin/fsnotifier" >> ${STAGEDIR}${DATADIR}/bin/idea.properties

.include <bsd.port.mk>
