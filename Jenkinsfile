pipeline {
    agent none

    stages {
        stage('Checkout') {
            agent any  // Use Jenkins host agent
            steps {
                git url: 'https://github.com/MuhammadAbideen/jenkinstutorial.git', branch: 'main'
            }
        }

        stage('Build') {
            agent {
                docker {
                    image 'ghcr.io/rikorose/gcc-cmake:latest'
                }
            }
            steps {
                sh '''
                    mkdir -p build
                    cd build
                    cmake .. -G "Unix Makefiles"
                    make
                '''
            }
        }

        stage('Run') {
            agent {
                docker {
                    image 'ghcr.io/rikorose/gcc-cmake:latest'
                }
            }
            steps {
                sh './build/mathdemo'
            }
        }
    }

    post {
        always {
            echo 'Pipeline completed.'
        }
        failure {
            echo 'Pipeline failed!'
        }
    }
}
