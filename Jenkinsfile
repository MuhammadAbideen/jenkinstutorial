pipeline {
    agent { label 'ubuntu' }
    stages {
        stage('Checkout') {
            steps {
                git url: 'https://github.com/MuhammadAbideen/jenkinstutorial.git', branch: 'main'
            }
        }
        stage('Install Dependencies') {
            steps {
                sh '''
                    sudo apt-get update
                    sudo apt-get install -y cmake g++ ninja-build
                '''
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