pipeline {
  agent any
  stages {
    stage('Cmake Generation') {
      steps {
        cmakeBuild(installation: 'AutoInstall', buildDir: 'out/build', buildType: 'Debug')
      }
    }

  }
}