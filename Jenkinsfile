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

  }
}