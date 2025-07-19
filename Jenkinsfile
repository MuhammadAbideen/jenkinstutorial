pipeline {
    agent {
        docker {
            image 'ghcr.io/rikorose/gcc-cmake:latest'
        }
    }
    stages {
        stage('Checkout') {
            steps {
                git url: 'https://github.com/MuhammadAbideen/jenkinstutorial.git', branch: 'main'
            }
        }
        stage('Build') {
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