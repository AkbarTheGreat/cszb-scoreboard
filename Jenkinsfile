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

    stage('Test') {
      parallel {
        stage('Debug Test') {
          steps {
            ctest(installation: 'AutoInstall', workingDir: 'out/build/Debug')
          }
        }

        stage('Release Test') {
          steps {
            ctest(installation: 'AutoInstall', workingDir: 'out/build/Release')
          }
        }

      }
    }

  }
}