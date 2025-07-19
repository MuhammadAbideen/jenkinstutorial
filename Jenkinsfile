pipeline {
    agent none

    stages {
        stage('Checkout') {
            agent any
            steps {
                cleanWs()
                git url: 'https://github.com/MuhammadAbideen/jenkinstutorial.git', branch: 'main'
            }
        }

        stage('Build') {
            agent {
                docker {
                    image 'ghcr.io/rikorose/gcc-cmake:latest'
                    args '-u root'
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
                    args '-u root'
                }
            }
            steps {
                sh '''
                    chmod +x build/mathdemo
                    ./build/mathdemo
                '''
            }
        }
    }

    post {
        always {
            echo 'Pipeline completed.'
            cleanWs()
        }
        success {
            echo 'Pipeline succeeded!'
        }
        failure {
            echo 'Pipeline failed!'
        }
    }
}
