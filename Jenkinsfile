pipeline {
  agent any
  stages {
    stage('Cmake Generation') {
      parallel {
        stage('Debug Cmake Generation') {
          steps {
            cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Debug', buildType: 'Debug')
          }
        }

        stage('Release Cmake Generation') {
          steps {
            cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/Release', buildType: 'Release')
          }
        }

      }
    }

    stage('Build') {
      parallel {
        stage('Debug Build') {
          steps {
            sh '''cd out/build/Debug
make all'''
          }
        }

        stage('Release Build') {
          steps {
            sh '''cd out/build/Release
make all'''
          }
        }

      }
    }

    stage('Test') {
      stages {
        stage('Debug Test') {
          steps {
            wrap(delegate: [$class: 'Xvnc', takeScreenshot: true, useXauthority: true]) {
              ctest(installation: 'AutoInstall', workingDir: 'out/build/Debug', arguments: '-T Test --output-on-failure --no-compress-output')
            }

          }
        }

        stage('Release Test') {
          steps {
            wrap(delegate: [$class: 'Xvnc', takeScreenshot: false, useXauthority: true]) {
              ctest(installation: 'AutoInstall', workingDir: 'out/build/Release', arguments: '-T Test --output-on-failure --no-compress-output')
            }

          }
        }

      }
    }

  }
  post {
    always {
      archiveArtifacts (
        artifacts: 'out/build/*/Testing/**/*.xml',
        fingerprint: true
      )
      xunit (
        testTimeMargin: '3000',
        thresholdMode: 1,
        thresholds: [
          skipped(failureThreshold: '0'),
          failed(failureThreshold: '0')
        ],
        tools: [CTest(
          pattern: 'out/build/*/Testing/**/*.xml',
          deleteOutputFiles: true,
          failIfNotNew: false,
          skipNoTestFiles: true,
          stopProcessingIfError: true
        )]
      )
      deleteDir()
    }
  }
}