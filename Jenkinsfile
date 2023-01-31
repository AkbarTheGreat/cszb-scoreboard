pipeline {
  agent any
  triggers {
    cron('00 04 * * *') //run at 4 am
  }

  stages {
    stage ('Build & Test') {
        parallel {
            stage('Lint') {
                when {
                    expression {
                        return runFullPipeline()
                    }
                }
                agent {
                    dockerfile {
                        filename 'Dockerfile.standard'
                    }
                }
                stages {
                    stage('Lint Cmake Generation') {
						steps {
							cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Linter', buildType: 'Debug',
									cmakeArgs: '-DSKIP_LINT=false -DCLANG_TIDY_ERRORS=true -DINTEGRATION_TEST=true'
							)
						}
					}
					stage('Lint Build') {
						steps {
							sh '''cd out/build/Linter
make all'''
						}
					}
                }
            }
            stage('Debug') {
                agent {
                    dockerfile {
                        filename 'Dockerfile.standard'
                    }
                }
                stages {
                    stage('Debug Cmake Generation') {
						steps {
							cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Debug', buildType: 'Debug',
									cmakeArgs: "-DSKIP_LINT=true -DINTEGRATION_TEST=${runFullPipeline()}"
							)
						}
					}
					stage('Debug Build') {
						steps {
							sh '''cd out/build/Debug
make -j2 all'''
						}
					}
					stage('Debug Test') {
						steps {
							retry(count: 3) {
								runTests('Release', runFullPipeline())
							}
						}
					}
					stage('Valgrind') {
						steps {
							valgrindRun(runFullPipeline())
	
							publishValgrind (
							failBuildOnInvalidReports: false,
							failBuildOnMissingReports: false,
							failThresholdDefinitelyLost: '',
							failThresholdInvalidReadWrite: '',
							failThresholdTotal: '',
							pattern: 'out/build/Debug/*.memcheck',
							publishResultsForAbortedBuilds: false,
							publishResultsForFailedBuilds: false,
							sourceSubstitutionPaths: '',
							unstableThresholdDefinitelyLost: '',
							unstableThresholdInvalidReadWrite: '',
							unstableThresholdTotal: ''
							)
						}
					}
                }
            }
            stage('Release') {
                agent {
                    dockerfile {
                        filename 'Dockerfile.standard'
                    }
                }
                stages {
                    stage('Release Cmake Generation') {
						steps {
							cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Release', buildType: 'Release',
									cmakeArgs: "-DSKIP_LINT=true -DINTEGRATION_TEST=${runFullPipeline()}"
							)
						}
					}
					stage('Release Build') {
						steps {
							sh '''cd out/build/Release
make -j2 all'''
						}
					}
					stage('Release Test') {
						steps {
							retry(count: 3) {
								runTests('Debug', runFullPipeline())
							}
						}
					}
                }
            }
            stage('MacOS') {
                when { expression { false } }
                agent {
                    dockerfile {
                        filename 'Dockerfile.osxcross'
                    }
                }
                stages {
                    stage('MacOS Cmake Generation') {
						environment {
							LD_LIBRARY_PATH = '/opt/osxcross/lib'
							OSXCROSS_SDK = 'darwin19'
							OSXCROSS_TARGET = 'darwin19'
							OSXCROSS_HOST = 'x86_64-apple-darwin19'
							OSXCROSS_TARGET_DIR = '/opt/osxcross'
							PATH = '/opt/osxcross/bin:$PATH'
						}
						steps {
							cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/osxcross', buildType: 'Release',
									cmakeArgs: '-DCMAKE_OSX_DEPLOYMENT_TARGET=10.12 -DCMAKE_TOOLCHAIN_FILE=/opt/osxcross/toolchain.cmake -DOPENSSL_ROOT_DIR=/opt/osxcross/macports/pkgs/opt/local/libexec/openssl3 -DINTEGRATION_TEST=false'
							)
						}
					}
					stage('MacOS Build') {
						environment {
							LD_LIBRARY_PATH = '/opt/osxcross/lib'
						}
						steps {
							sh '''cd out/build/osxcross
export PATH=/opt/osxcross/bin:$PATH
make scoreboard_proto cszb-scoreboard'''
						}
					}
                }
            }
        }
    }
    stage('Coverage') {
      when {
        expression {
          return runFullPipeline()
        }
      }
      steps {
        cmakeBuild(
          installation: 'AutoInstall',
          buildDir: 'out/build/Coverage',
          buildType: 'Debug',
          cleanBuild: true,
          cmakeArgs: '-DENABLE_CODE_COVERAGE=true -DCMAKE_CXX_FLAGS=-DSCOREBOARD_ENABLE_LOGGING')
        retry(count: 3) {
          sh '''cd out/build/Coverage
            make -j3 all cszb-scoreboard-xml-coverage
          '''
        }
        cobertura(sourceEncoding: 'ASCII', coberturaReportFile: 'out/build/Coverage/cszb-scoreboard-xml-coverage.xml')
      }
    }
    
  }
  post {
    always {
      archiveArtifacts(artifacts: 'out/build/*/Testing/**/*.xml', fingerprint: true)
      xunit(testTimeMargin: '3000', thresholdMode: 1, thresholds: [
          skipped(failureThreshold: '0'),
          failed(failureThreshold: '0')
        ], tools: [CTest(
          pattern: 'out/build/*/Testing/**/*.xml',
          deleteOutputFiles: true,
          failIfNotNew: false,
          skipNoTestFiles: true,
          stopProcessingIfError: true
        )])
      deleteDir()
    }
  }
}

def valgrindRun(isIntegration) {
    if (isIntegration) {
        wrap(delegate: [$class: 'Xvnc', takeScreenshot: true, useXauthority: true]) {
            executeValgrind()
        }
    } else {
        executeValgrind()
    }
}

def executeValgrind() {
    runValgrind (
        childSilentAfterFork: false,
        excludePattern: 'donotexcludeanything',
        generateSuppressions: true,
        ignoreExitCode: true,
        includePattern: 'out/build/Debug/*Test',
        outputDirectory: 'out/build/Debug',
        outputFileEnding: '.memcheck',
        programOptions: ' ',
        removeOldReports: true,
        suppressionFiles: '../../../etc/valgrind_suppressions.supp',
        tool: [$class: 'ValgrindToolMemcheck',
            leakCheckLevel: 'full',
            showReachable: true,
            trackOrigins: true,
            undefinedValueErrors: true],
        traceChildren: true,
        valgrindExecutable: '/usr/bin/valgrind',
        valgrindOptions: ' ',
        workingDirectory: 'out/build/Debug'
    )
}

def runTests(testDir, isIntegration) {
    if (isIntegration) {
        wrap(delegate: [$class: 'Xvnc', takeScreenshot: false, useXauthority: true]) {
            ctest(installation: 'AutoInstall', workingDir: "out/build/${testDir}", arguments: '-T Test --output-on-failure --no-compress-output')
        }
    } else {
        ctest(installation: 'AutoInstall', workingDir: "out/build/${testDir}", arguments: '-T Test --output-on-failure --no-compress-output')
    }
}

// Originally suggested by https://stackoverflow.com/a/58624083 to determine when a job is started by a timer, expanded to cover all cases where we want to run a full pipeline.
@NonCPS
def runFullPipeline() {
    def runFull = false
    try {
        def buildCauses = currentBuild.getBuildCauses()
        for ( buildCause in buildCauses ) {
            if (buildCause != null) {
                def causeDescription = buildCause.shortDescription
                echo "shortDescription: ${causeDescription}"
                if (causeDescription.contains("Started by timer")) {
                    runFull = true
                }
                if (causeDescription.contains("Started by user")) {
                    runFull = true
                }
            }
        }
    } catch(theError) {
        echo "Error getting build cause"
    }

    return runFull
}

