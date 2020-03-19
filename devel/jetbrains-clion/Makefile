# $FreeBSD$

PORTNAME=	clion
PORTVERSION=	2019.3.5
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

RUN_DEPENDS=	cmake>=3.14.3:devel/cmake \
		intellij-fsnotifier>0:java/intellij-fsnotifier \
		intellij-pty4j>0:java/intellij-pty4j

USES=		linux python:run shebangfix

USE_JAVA=	yes
JAVA_VERSION=	11+

SHEBANG_FILES=	bin/printenv.py bin/restart.py

NO_ARCH=	yes
NO_BUILD=	yes

WRKSRC=		${WRKDIR}/clion-${PORTVERSION}

SUB_FILES=	clion clion.desktop pkg-message

CLION_CLANG=	bin/clang/linux

do-install:
# Linux/Windows/OS X only so remove them
	@${RM} -r ${WRKSRC}/bin/fsnotifier \
		${WRKSRC}/bin/fsnotifier-arm \
		${WRKSRC}/bin/fsnotifier64 \
		${WRKSRC}/bin/cmake/linux \
		${WRKSRC}/bin/gdb/linux \
		${WRKSRC}/bin/lldb/linux \
		${WRKSRC}/lib/pty4j-native \
		${WRKSRC}/plugins/performanceTesting/bin
# Remove bundled linux JetBrains Runtime and OpenJDK
	@${RM} -r ${WRKSRC}/jbr ${WRKSRC}/jre64
# Use cmake binaries provided by devel/cmake
	${MKDIR} ${WRKSRC}/bin/cmake/linux/bin
.for f in ccmake cmake cpack ctest
	${LN} -sf ${PREFIX}/bin/${f} ${WRKSRC}/bin/cmake/linux/bin/${f}
.endfor

	${MKDIR} ${STAGEDIR}${DATADIR}
	@(cd ${WRKSRC} && ${COPYTREE_SHARE} . ${STAGEDIR}${DATADIR} \
		"! -path \./${CLION_CLANG}/* ! -name *\.so ! -name *\.dll ! -name *\.dylib ! -name *\.pdb ! -name *\.sh")
	@(cd ${WRKSRC} && ${COPYTREE_BIN} ${CLION_CLANG} ${STAGEDIR}${DATADIR})
	@(cd ${WRKSRC} && ${COPYTREE_BIN} . ${STAGEDIR}${DATADIR} "-name *\.sh")
	${INSTALL_SCRIPT} ${WRKDIR}/clion ${STAGEDIR}${PREFIX}/bin/clion
	${INSTALL_MAN} ${FILESDIR}/clion.1 ${STAGEDIR}${PREFIX}/man/man1
	${INSTALL_DATA} ${WRKDIR}/clion.desktop ${STAGEDIR}${PREFIX}/share/applications/
# Use fsnotifier replacement provided by java/intellij-fsnotifier
	${ECHO} "idea.filewatcher.executable.path=${PREFIX}/intellij/bin/fsnotifier" >> ${STAGEDIR}${DATADIR}/bin/idea.properties
# Fix slow render
	${ECHO} "-Dsun.java2d.xrender=false" >> ${STAGEDIR}${DATADIR}/bin/clion.vmoptions
	${ECHO} "-Dsun.java2d.xrender=false" >> ${STAGEDIR}${DATADIR}/bin/clion64.vmoptions

.include <bsd.port.mk>
