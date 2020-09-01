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

        stage('MacOS Cmake Generation') {
          environment {
            OSXCROSS_SDK = 'darwin19'
            OSXCROSS_TARGET = 'darwin19'
            OSXCROSS_HOST = 'x86_64-apple-darwin19'
            OSXCROSS_TARGET_DIR = '/opt/osxcross'
          }
          steps {
            cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build/osxcross', buildType: 'Release',
            cmakeArgs: '-DCMAKE_OSX_DEPLOYMENT_TARGET=10.12 -DCMAKE_TOOLCHAIN_FILE=/opt/osxcross/toolchain.cmake'
            )
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
    stage('Valgrind') {
      steps {
        wrap(delegate: [$class: 'Xvnc', takeScreenshot: true, useXauthority: true]) {
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
            suppressionFiles: '',
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