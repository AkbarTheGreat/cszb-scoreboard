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
      parallel {
        stage('Debug Test') {
          steps {
            wrap([$class: 'Xvnc', takeScreenshot: false, useXauthority: true]) {
              ctest(installation: 'AutoInstall', workingDir: 'out/build/Debug')
            }
          }
        }

        stage('Release Test') {
          steps {
            wrap([$class: 'Xvnc', takeScreenshot: false, useXauthority: true]) {
              ctest(installation: 'AutoInstall', workingDir: 'out/build/Release')
            }
          }
        }

      }
    }
    
    stage('Valgrind') {
      steps {
        runValgrind (
          childSilentAfterFork: true,
          excludePattern: '',
          generateSuppressions: true,
          ignoreExitCode: true,
          includePattern: 'bin/test_leak',
          outputDirectory: '',
          outputFileEnding: '.memcheck',
          programOptions: '',
          removeOldReports: true,
          suppressionFiles: '',
          tool: [$class: 'ValgrindToolMemcheck',
                          leakCheckLevel: 'full',
                          showReachable: true,
                          trackOrigins: true,
                          undefinedValueErrors: true],
          traceChildren: true,
          valgrindExecutable: '/usr/bin/valgrind',
          valgrindOptions: '',
          workingDirectory: 'out/build/Debug'
        )
  
        publishValgrind (
          failBuildOnInvalidReports: false,
          failBuildOnMissingReports: false,
          failThresholdDefinitelyLost: '',
          failThresholdInvalidReadWrite: '',
          failThresholdTotal: '',
          pattern: '*.memcheck',
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