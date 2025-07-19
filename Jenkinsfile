pipeline {
    agent any

    environment {
        // Customize these variables according to your needs
        BUILD_DIR = 'build'
    }

    stages {
        stage('Checkout') {
            steps {
                // Clean workspace before checking out
                cleanWs()
                
                // Checkout your repository
                checkout scm
            }
        }

        stage('Install Dependencies') {
            steps {
                script {
                    // Install required packages on Ubuntu
                    sh '''
                        sudo apt-get update
                        sudo apt-get install -y cmake g++ build-essential
                    '''
                }
            }
        }

        stage('Build') {
            steps {
                script {
                    // Create build directory and run CMake
                    sh """
                        mkdir -p ${BUILD_DIR}
                        cd ${BUILD_DIR}
                        cmake ..
                        cmake --build . --config Release
                    """
                }
            }
        }

        stage('Run') {
            steps {
                script {
                    // Run the compiled executable
                    sh """
                        cd ${BUILD_DIR}
                        ./mathdemo
                    """
                }
            }
        }
    }

    post {
        success {
            echo 'Pipeline completed successfully!'
        }
        failure {
            echo 'Pipeline failed. Please check the logs for details.'
        }
        always {
            // Clean up workspace after build
            cleanWs()
        }
    }
}
