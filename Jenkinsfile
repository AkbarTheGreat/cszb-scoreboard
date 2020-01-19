pipeline {
  agent any
  stages {
    stage('Cmake Generation') {
      steps {
        cmakeBuild(installation: '/usr/bin/cmake', buildDir: 'out/build')
      }
    }

  }
}