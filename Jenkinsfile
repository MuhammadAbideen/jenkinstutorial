pipeline {
    agent {
        docker {
            image 'ghcr.io/MuhammadAbideen/gcc-cmake-python:latest'
            args '--user root'
        }
    }
    environment {
        OPENAI_API_KEY = credentials('openai-api-key')
        GITHUB_TOKEN = credentials('github-pat')
    }
    stages {
        stage('Checkout') {
            steps {
                checkout scmGit(
                    branches: [[name: '*/main']],
                    userRemoteConfigs: [[url: 'https://github.com/MuhammadAbideen/jenkinstutorial.git']]
                )
            }
        }
        stage('SonarQube Analysis') {
            environment {
                SCANNER_HOME = tool 'SonarQubeScanner'
            }
            steps {
                withSonarQubeEnv('SonarQubeLocal') {
                    sh '''
                        ${SCANNER_HOME}/bin/sonar-scanner \
                        -Dsonar.projectKey=mathdemo \
                        -Dsonar.projectName=MathDemo \
                        -Dsonar.projectVersion=1.0 \
                        -Dsonar.sources=src \
                        -Dsonar.language=cpp \
                        -Dsonar.sourceEncoding=UTF-8 \
                        -Dsonar.cfamily.build-wrapper-output=build_wrapper_output
                    '''
                }
            }
        }
        stage('Quality Gate') {
            steps {
                timeout(time: 10, unit: 'MINUTES') {
                    waitForQualityGate abortPipeline: true
                }
            }
        }
        stage('Code Review') {
            steps {
                sh '''
                    python3 reviewer.py > review_output.txt
                '''
                script {
                    def review = readFile 'review_output.txt'
                    def prNumber = sh(script: """
                        curl -s -H "Authorization: token ${GITHUB_TOKEN}" \
                        -H "Accept: application/vnd.github.v3+json" \
                        https://api.github.com/repos/MuhammadAbideen/jenkinstutorial/pulls?head=MuhammadAbideen:${env.BRANCH_NAME}&base=main | \
                        jq -r '.[0].number'
                    """, returnStdout: true).trim()
                    if (prNumber != 'null' && prNumber != '') {
                        sh """
                            curl -X POST -H "Authorization: token ${GITHUB_TOKEN}" \
                            -H "Accept: application/vnd.github.v3+json" \
                            https://api.github.com/repos/MuhammadAbideen/jenkinstutorial/pulls/${prNumber}/comments \
                            -d '{"body": "${review.replace('"', '\\"').replace('\n', '\\n')}"}'
                        """
                    } else {
                        echo 'No open PR found for this branch. Skipping comment.'
                    }
                }
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
                sh '''
                    chmod +x build/mathdemo
                    ./build/mathdemo
                '''
            }
        }
        stage('Clean Workspace') {
            steps {
                cleanWs()
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