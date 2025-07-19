pipeline {
    agent any  // Run on any available agent (controller in this case)
    stages {
        stage('Checkout') {
            steps {
                git url: 'https://github.com/MuhammadAbideen/jenkinstutorial.git', branch: 'main'
            }
        }
        stage('Install Dependencies') {
            steps {
                sh '''
                    apt-get update
                    apt-get install -y cmake g++ make
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