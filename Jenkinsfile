pipeline {
  agent any
  stages {
    stage('Cmake Generation') {
      parallel {
        stage('Lint Build Cmake Generation') {
          when {
            expression {
              return isJobStartedByTimer()
            }

          }
          steps {
            cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Linter', buildType: 'Debug', cmakeArgs: '-DSKIP_LINT=false -DCLANG_TIDY_ERRORS=true')
          }
        }

        stage('Debug Cmake Generation') {
          steps {
            cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Debug', buildType: 'Debug', cmakeArgs: '-DSKIP_LINT=true')
          }
        }

        stage('Release Cmake Generation') {
          steps {
            cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Release', buildType: 'Release', cmakeArgs: '-DSKIP_LINT=true')
          }
        }

        stage('MacOS Cmake Generation') {
          environment {
            OSXCROSS_SDK = 'darwin19'
            OSXCROSS_TARGET = 'darwin19'
            OSXCROSS_HOST = 'x86_64-apple-darwin19'
            OSXCROSS_TARGET_DIR = '/opt/osxcross'
          }
          steps {
            cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/osxcross', buildType: 'Release', cmakeArgs: '-DCMAKE_OSX_DEPLOYMENT_TARGET=10.12 -DCMAKE_TOOLCHAIN_FILE=/opt/osxcross/toolchain.cmake')
          }
        }

      }
    }

    stage('Build') {
      parallel {
        stage('Lint Build') {
          when {
            expression {
              return isJobStartedByTimer()
            }

          }
          steps {
            sh '''cd out/build/Linter
make all'''
          }
        }

        stage('Debug Build') {
          steps {
            sh '''cd out/build/Debug
make -j2 all'''
          }
        }

        stage('Release Build') {
          steps {
            sh '''cd out/build/Release
make -j2 all'''
          }
        }

        stage('MacOS Build') {
          steps {
            sh '''cd out/build/osxcross
make cszb-scoreboard'''
          }
        }

      }
    }

    stage('Debug Test') {
      steps {
        retry(count: 3) {
          wrap(delegate: [$class: 'Xvnc', takeScreenshot: true, useXauthority: true]) {
            ctest(installation: 'AutoInstall', workingDir: 'out/build/Debug', arguments: '-T Test --output-on-failure --no-compress-output')
          }

        }

      }
    }

    stage('Release Test') {
      steps {
        retry(count: 3) {
          wrap(delegate: [$class: 'Xvnc', takeScreenshot: false, useXauthority: true]) {
            ctest(installation: 'AutoInstall', workingDir: 'out/build/Release', arguments: '-T Test --output-on-failure --no-compress-output')
          }

        }

      }
    }

    stage('Coverage') {
      steps {
        cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Coverage', buildType: 'Debug', cleanBuild: true, cmakeArgs: '-DENABLE_CODE_COVERAGE=true')
        sh '''sh \'\'\'cd out/build/osxcross
make cszb-scoreboard-xml-coverage\'\'\'
'''
        cobertura(sourceEncoding: 'ASCII', coberturaReportFile: 'out/build/Coverage')
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
      triggers {
        cron('00 02 * * *')
      }
    }