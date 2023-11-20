pipeline {
  agent {
    node {
      label 'scoreboard-worker'
    }
  }
  triggers {
    cron('00 04 * * *') //run at 4 am
  }

  stages {
    stage ('Docker Builds') {
      parallel {
        stage('Standard Docker Build') {
          steps {
            sh """docker buildx create --name imagebuilder \\
                | --driver=remote tcp://buildkit:1234 \\
                | --driver-opt=cacert=/certs/ca.pem,cert=/certs/cert.pem,key=/certs/key.pem \\
                | --bootstrap --use""".stripMargin()
            sh """docker buildx build -t \\
                | docker.akbar.dev/akbarthegreat/scoreboard-testing-standard:${BRANCH_NAME} \\
                | --target=standard_build \\
                | --cache-to=type=registry,ref=docker.akbar.dev/akbarthegreat/scoreboard-build-cache-standard-${BRANCH_NAME} \\
                | --cache-from=type=registry,ref=docker.akbar.dev/akbarthegreat/scoreboard-build-cache-standard-${BRANCH_NAME} \\
                | --push . """.stripMargin()
          }
        } // Standard Docker
        stage('Osxcross Docker Build') {
          steps {
            sh """docker buildx create --name imagebuilder \\
                | --driver=remote tcp://buildkit:1234 \\
                | --driver-opt=cacert=/certs/ca.pem,cert=/certs/cert.pem,key=/certs/key.pem \\
                | --bootstrap --use""".stripMargin()
            sh """docker buildx build -t \\
                | docker.akbar.dev/akbarthegreat/scoreboard-testing-macos:${BRANCH_NAME} \\
                | --target=macos_build \\
                | --cache-to=type=registry,ref=docker.akbar.dev/akbarthegreat/scoreboard-build-cache-macos-${BRANCH_NAME} \\
                | --cache-from=type=registry,ref=docker.akbar.dev/akbarthegreat/scoreboard-build-cache-macos-${BRANCH_NAME} \\
                | --push . """.stripMargin()
          }
        } // Osx Docker
      } // Parallel block
    } // Docker Builds
    stage ('Build & Test') {
      parallel {
        stage('Lint') {
          when {
            expression {
              return runFullPipeline()
            }
          }
          agent {
            kubernetes {
              defaultContainer 'scoreboard'
              yaml """kind: Pod
                     |metadata:
                     |  labels:
                     |    scoreboard: lint
                     |spec:
                     |  imagePullSecrets:
                     |  - name: local-cred
                     |  containers:
                     |  - name: scoreboard
                     |    image: docker.akbar.dev/akbarthegreat/scoreboard-testing-standard:${BRANCH_NAME}
                     |    imagePullPolicy: Always
                     |    resources:
                     |      requests:
                     |        memory: 2.5Gi
                     |      limits:
                     |        memory: 2.5Gi
                     |    command:
                     |    - sleep
                     |    args:
                     |    - 99d
                     |  affinity:
                     |    nodeAffinity:
                     |      requiredDuringSchedulingIgnoredDuringExecution:
                     |        nodeSelectorTerms:
                     |        - matchExpressions:
                     |          - key: node-class
                     |            operator: NotIn
                     |            values:
                     |            - slow""".stripMargin()
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
                     |make -j6 all'''.stripMargin()
              }
            }
          }
        } // Lint
        stage('Debug') {
          agent {
            kubernetes {
              defaultContainer 'scoreboard'
              yaml """kind: Pod
                     |metadata:
                     |  labels:
                     |    scoreboard: debug
                     |spec:
                     |  imagePullSecrets:
                     |  - name: local-cred
                     |  containers:
                     |  - name: scoreboard
                     |    image: docker.akbar.dev/akbarthegreat/scoreboard-testing-standard:${BRANCH_NAME}
                     |    imagePullPolicy: Always
                     |    resources:
                     |      requests:
                     |        memory: 1.5Gi
                     |      limits:
                     |        memory: 1.5Gi
                     |    command:
                     |    - sleep
                     |    args:
                     |    - 99d
                     |  affinity:
                     |    nodeAffinity:
                     |      requiredDuringSchedulingIgnoredDuringExecution:
                     |        nodeSelectorTerms:
                     |        - matchExpressions:
                     |          - key: node-class
                     |            operator: NotIn
                     |            values:
                     |            - slow""".stripMargin()
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
                     |make -j6 all'''.stripMargin()
              }
            }
            stage('Debug Test') {
              steps {
                retry(count: 3) {
                  runTests('Debug', runFullPipeline())
                }
              }
              post {
                always {
                  archiveArtifacts(artifacts: 'out/build/Debug/Testing/**/*.xml', fingerprint: true)
                  xunit(testTimeMargin: '3000', thresholdMode: 1, thresholds: [
                        skipped(failureThreshold: '0'),
                        failed(failureThreshold: '0')
                        ], tools: [CTest(
                        pattern: 'out/build/Debug/Testing/**/*.xml',
                        deleteOutputFiles: false,
                        failIfNotNew: false,
                        skipNoTestFiles: true,
                        stopProcessingIfError: true
                        )])
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
        } // Debug
        stage('Release') {
          agent {
            kubernetes {
              defaultContainer 'scoreboard'
              yaml """kind: Pod
                     |metadata:
                     |  labels:
                     |    scoreboard: release
                     |spec:
                     |  imagePullSecrets:
                     |  - name: local-cred
                     |  containers:
                     |  - name: scoreboard
                     |    image: docker.akbar.dev/akbarthegreat/scoreboard-testing-standard:${BRANCH_NAME}
                     |    imagePullPolicy: Always
                     |    resources:
                     |      requests:
                     |        memory: 1.5Gi
                     |      limits:
                     |        memory: 1.5Gi
                     |    command:
                     |    - sleep
                     |    args:
                     |    - 99d
                     |  affinity:
                     |    nodeAffinity:
                     |      requiredDuringSchedulingIgnoredDuringExecution:
                     |        nodeSelectorTerms:
                     |        - matchExpressions:
                     |          - key: node-class
                     |            operator: NotIn
                     |            values:
                     |            - slow""".stripMargin()
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
                     |make -j6 all'''.stripMargin()
              }
            }
            stage('Release Test') {
              steps {
                retry(count: 3) {
                  runTests('Release', runFullPipeline())
                }
              }
              post {
                always {
                  archiveArtifacts(artifacts: 'out/build/Release/Testing/**/*.xml', fingerprint: true)
                  xunit(testTimeMargin: '3000', thresholdMode: 1, thresholds: [
                    skipped(failureThreshold: '0'),
                    failed(failureThreshold: '0')
                    ], tools: [CTest(
                      pattern: 'out/build/Release/Testing/**/*.xml',
                      deleteOutputFiles: false,
                      failIfNotNew: false,
                      skipNoTestFiles: true,
                      stopProcessingIfError: true
                  )])
                }
              }
            }
          }
        } // Release
        stage('MacOS') {
          agent {
            kubernetes {
              defaultContainer 'scoreboard'
              yaml """kind: Pod
                     |metadata:
                     |  labels:
                     |    scoreboard: macos
                     |spec:
                     |  imagePullSecrets:
                     |  - name: local-cred
                     |  containers:
                     |  - name: scoreboard
                     |    image: docker.akbar.dev/akbarthegreat/scoreboard-testing-macos:${BRANCH_NAME}
                     |    imagePullPolicy: Always
                     |    resources:
                     |      requests:
                     |        memory: 1.5Gi
                     |      limits:
                     |        memory: 1.5Gi
                     |    command:
                     |    - sleep
                     |    args:
                     |    - 99d
                     |  affinity:
                     |    nodeAffinity:
                     |      requiredDuringSchedulingIgnoredDuringExecution:
                     |        nodeSelectorTerms:
                     |        - matchExpressions:
                     |          - key: node-class
                     |            operator: NotIn
                     |            values:
                     |            - slow""".stripMargin()
            }
          }
          stages {
            stage('MacOS Cmake Generation') {
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
                     |export PATH=/opt/osxcross/bin:$PATH
                     |make -j6 scoreboard_proto
                     |make -j6 cszb-scoreboard'''.stripMargin()
              }
            }
          }
        } // MacOS
        stage('Coverage') {
          when {
            expression {
              return runFullPipeline()
            }
          }
          agent {
            kubernetes {
              defaultContainer 'scoreboard'
              yaml """kind: Pod
                     |metadata:
                     |  labels:
                     |    scoreboard: coverage
                     |spec:
                     |  imagePullSecrets:
                     |  - name: local-cred
                     |  containers:
                     |  - name: scoreboard
                     |    image: docker.akbar.dev/akbarthegreat/scoreboard-testing-standard:${BRANCH_NAME}
                     |    imagePullPolicy: Always
                     |    resources:
                     |      requests:
                     |        memory: 2Gi
                     |      limits:
                     |        memory: 2Gi
                     |    command:
                     |    - sleep
                     |    args:
                     |    - 99d
                     |  affinity:
                     |    nodeAffinity:
                     |      requiredDuringSchedulingIgnoredDuringExecution:
                     |        nodeSelectorTerms:
                     |        - matchExpressions:
                     |          - key: node-class
                     |            operator: NotIn
                     |            values:
                     |            - slow""".stripMargin()
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
                   |make -j6 all cszb-scoreboard-xml-coverage'''.stripMargin()
            }
            cobertura(sourceEncoding: 'ASCII', coberturaReportFile: 'out/build/Coverage/cszb-scoreboard-xml-coverage.xml')
          }
        } // Coverage
      } // Parallel block
    } // Build & Test
  }
}

def valgrindRun(isIntegration) {
    if (isIntegration) {
        sh 'sudo supervisord -c /supervisord.conf'
        executeValgrind()
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
        sh 'sudo supervisord -c /supervisord.conf'
        ctest(installation: 'AutoInstall', workingDir: "out/build/${testDir}", arguments: '-T Test --output-on-failure --no-compress-output')
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

