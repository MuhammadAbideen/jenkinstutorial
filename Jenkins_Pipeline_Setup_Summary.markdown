# Jenkins Pipeline Setup Summary

This document summarizes the work carried out to configure a Jenkins pipeline for the `https://github.com/gitusername/jenkinstutorial.git` repository, which contains a C++ project (`mathdemo`) built using CMake and the `ghcr.io/rikorose/gcc-cmake:latest` Docker image. The pipeline runs on a local Jenkins instance (`http://localhost:8080`) in a Docker container on a Windows machine, with webhook support via ngrok. The work includes resolving pipeline errors, enabling auto-triggering on commits, integrating SonarQube for code quality, and adding an LLM-based code review stage with PR comments.

## Environment
- **Jenkins**: Running in Docker (`jenkins/jenkins:lts`) with ports `8080:8080` and `50000:50000`, volume mounts for `jenkins_home` and `/var/run/docker.sock`, and `--user root`.
- **Repository**: `https://github.com/gitusername/jenkinstutorial.git`, `main` branch, commit `6ba1152cac622ed309d8bf153071a314dd3f15ef`.
- **Project**: C++ application (`mathdemo`) with files:
  - `CMakeLists.txt`
  - `src/main.cpp`
  - `src/mathlib/mathlib.h`
  - `src/mathlib/mathlib.cpp`
  - `.gitignore`
  - `README.md`
  - `Jenkinsfile`
  - `reviewer.py` (added later for LLM review)
- **Docker Host**: Windows machine with Docker Desktop and WSL for local testing.
- **ngrok**: Used to expose Jenkins for GitHub webhooks.

## Work Carried Out

### 1. Resolving Initial Pipeline Errors
**Issue**: The pipeline failed in the `Build` stage with `docker: not found` errors due to missing Docker CLI in the Jenkins container.

**Solution**:
- **Install Docker CLI**:
  - Accessed the Jenkins container:
    ```powershell
    docker exec -it jenkins-local bash
    ```
  - Installed Docker CLI:
    ```bash
    apt-get update
    apt-get install -y docker.io
    ```
  - Verified:
    ```bash
    docker --version
    docker pull ghcr.io/rikorose/gcc-cmake:latest
    ```
- **Custom Jenkins Image** (recommended for consistency):
  - Created a `Dockerfile` in `C:\Users\username\jenkins-docker`:
    ```dockerfile
    FROM jenkins/jenkins:lts
    USER root
    RUN apt-get update && apt-get install -y docker.io cmake g++ make git
    USER jenkins
    ```
  - Built and ran:
    ```powershell
    cd C:\Users\username\jenkins-docker
    docker build -t my-jenkins .
    docker stop jenkins-local
    docker rm jenkins-local
    docker run -d --name jenkins-local -p 8080:8080 -p 50000:50000 -v jenkins_home:/var/jenkins_home -v /var/run/docker.sock:/var/run/docker.sock --user root my-jenkins
    ```

**Issue**: The `post` block failed with `hudson.AbortException: Attempted to execute a step that requires a node context while ‘agent none’ was specified`.

**Solution**:
- Moved `cleanWs` to a dedicated `Clean Workspace` stage to ensure node context.
- Updated `Jenkinsfile` to use a consistent Docker agent:
  ```groovy
  pipeline {
      agent {
          docker {
              image 'ghcr.io/rikorose/gcc-cmake:latest'
              args '--user root'
          }
      }
      stages {
          stage('Checkout') {
              steps {
                  checkout scmGit(
                      branches: [[name: '*/main']],
                      userRemoteConfigs: [[url: 'https://github.com/gitusername/jenkinstutorial.git']]
                  )
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
  ```
- Pushed changes:
  ```powershell
  cd C:\Users\username\jenkinstutorial
  git add Jenkinsfile
  git commit -m "Fix cleanWs and ensure Docker agent consistency"
  git push origin main
  ```

**Outcome**:
- `Checkout`, `Build`, and `Run` stages succeeded, producing:
  ```
  [2025-07-19 08:47:48.885] [info] Starting math operations demo
  [2025-07-19 08:47:48.885] [info] 5 + 3 = 8
  [2025-07-19 08:47:48.885] [info] 10 - 4 = 6
  [2025-07-19 08:47:48.885] [info] 15 / 3 = 5
  [2025-07-19 08:47:48.886] [error] Error occurred: Division by zero is not allowed
  [2025-07-19 08:47:48.886] [info] Demo completed successfully
  ```
- The `Clean Workspace` stage resolved the `post` block error.

### 2. Enabling Auto-Trigger on Commits
**Objective**: Auto-trigger the pipeline on pushes to the `main` branch.

**Solution**:
- **Installed ngrok**:
  - Downloaded ngrok to `C:\Users\username\ngrok`.
  - Started ngrok:
    ```powershell
    cd C:\Users\username\ngrok
    ngrok http 8080
    ```
  - Noted the public URL (e.g., `https://a2dfd0c9.ngrok.io`).
- **Configured GitHub Webhook**:
  - In GitHub, went to `https://github.com/gitusername/jenkinstutorial` > **Settings** > **Webhooks** > **Add webhook**.
  - Set:
    - **Payload URL**: `https://a2dfd0c9.ngrok.io/github-webhook/`
    - **Content type**: `application/json`
    - **Events**: `Just the push event`
    - **Active**: Enabled
  - Verified webhook delivery.
- **Enabled Jenkins Trigger**:
  - In Jenkins, opened the “abcd” pipeline configuration.
  - Checked **GitHub hook trigger for GITScm polling**.
  - Saved changes.
- **Tested Auto-Trigger**:
  - Made a test commit:
    ```powershell
    cd C:\Users\username\jenkinstutorial
    echo "Test webhook trigger" >> README.md
    git add README.md
    git commit -m "Test webhook trigger"
    git push origin main
    ```
  - Confirmed the pipeline triggered automatically.

**Outcome**:
- Pipeline auto-triggers on `main` branch commits via the GitHub webhook.

### 3. Integrating SonarQube
**Objective**: Add SonarQube for code quality analysis.

**Solution**:
- **Ran SonarQube**:
  - Started SonarQube in Docker:
    ```powershell
    docker pull sonarqube:community
    docker run -d --name sonarqube -p 9000:9000 sonarqube:community
    ```
  - Accessed `http://localhost:9000`, logged in (`admin`/`admin`), and changed the password.
- **Configured SonarQube**:
  - Created a project: **Project Key**: `mathdemo`, **Display Name**: `MathDemo`.
  - Generated a token (e.g., `sqa_1234567890abcdef`).
  - Added a webhook: `https://a2dfd0c9.ngrok.io/sonarqube-webhook/`.
- **Configured Jenkins**:
  - Installed **SonarQube Scanner for Jenkins** plugin.
  - Added SonarQube server in **Manage Jenkins** > **Configure System**:
    - **Name**: `SonarQubeLocal`
    - **Server URL**: `http://host.docker.internal:9000`
    - **Server authentication token**: `sonarqube-token` (using the generated token).
  - Configured SonarQube Scanner in **Global Tool Configuration**:
    - **Name**: `SonarQubeScanner`
    - **Install automatically**: Latest version.
- **Updated Jenkinsfile**:
  - Added `SonarQube Analysis` and `Quality Gate` stages:
    ```groovy
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
    ```
- **Tested**:
  - Pushed changes and verified SonarQube analysis at `http://localhost:9000`.
  - Confirmed quality gate status and pipeline continuation.

**Outcome**:
- SonarQube analyzes C++ code (`src/`), reports bugs, vulnerabilities, and code smells, and enforces quality gates.

### 4. Adding LLM-Based Code Review
**Objective**: Integrate an LLM code review stage using `reviewer.py` and post results as PR comments.

**Solution**:
- **Created Custom Docker Image**:
  - Created a `Dockerfile` in `C:\Users\username\docker`:
    ```dockerfile
    FROM ghcr.io/rikorose/gcc-cmake:latest
    USER root
    RUN apt-get update && apt-get install -y python3 python3-pip jq
    RUN pip3 install openai
    ```
  - Built and pushed:
    ```powershell
    cd C:\Users\username\docker
    docker build -t username/gcc-cmake-python:latest .
    docker login ghcr.io -u gitusername
    docker tag username/gcc-cmake-python:latest ghcr.io/gitusername/gcc-cmake-python:latest
    docker push ghcr.io/gitusername/gcc-cmake-python:latest
    ```
- **Added Credentials**:
  - In Jenkins, added:
    - **OpenAI API Key**: `openai-api-key` (e.g., `sk-1234567890abcdef`).
    - **GitHub PAT**: `github-pat` (e.g., `ghp_1234567890abcdef`) with `repo` scope.
- **Added reviewer.py**:
  - Ensured `reviewer.py` is in the repository:
    ```python
    import openai
    import os

    openai.api_key = os.getenv("OPENAI_API_KEY")

    with open("src/main.cpp", "r") as f:
        code = f.read()

    response = openai.ChatCompletion.create(
        model="gpt-4",
        messages=[
            {"role": "system", "content": "You are a professional C++ code reviewer."},
            {"role": "user", "content": f"Please review the following C++ code:\n{code}"}
        ]
    )

    print("==== LLM Review ====")
    print(response['choices'][0]['message']['content'])
    ```
  - Pushed if missing:
    ```powershell
    cd C:\Users\username\jenkinstutorial
    git add reviewer.py
    git commit -m "Add reviewer.py for LLM code review"
    git push origin main
    ```
- **Updated Jenkinsfile**:
  - Added `Code Review` stage to run `reviewer.py` and post to PR:
    ```groovy
    environment {
        OPENAI_API_KEY = credentials('openai-api-key')
        GITHUB_TOKEN = credentials('github-pat')
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
                    https://api.github.com/repos/gitusername/jenkinstutorial/pulls?head=gitusername:${env.BRANCH_NAME}&base=main | \
                    jq -r '.[0].number'
                """, returnStdout: true).trim()
                if (prNumber != 'null' && prNumber != '') {
                    sh """
                        curl -X POST -H "Authorization: token ${GITHUB_TOKEN}" \
                        -H "Accept: application/vnd.github.v3+json" \
                        https://api.github.com/repos/gitusername/jenkinstutorial/pulls/${prNumber}/comments \
                        -d '{"body": "${review.replace('"', '\\"').replace('\n', '\\n')}"}'
                    """
                } else {
                    echo 'No open PR found for this branch. Skipping comment.'
                }
            }
        }
    }
    ```
- **Tested PR Workflow**:
  - Created a branch and PR:
    ```powershell
    cd C:\Users\username\jenkinstutorial
    git checkout -b feature/review-test
    echo "Test PR comment" >> README.md
    git add README.md
    git commit -m "Test change for PR review"
    git push origin feature/review-test
    ```
  - Opened a PR from `feature/review-test` to `main`.
  - Verified the pipeline triggered and posted the LLM review as a PR comment.

**Outcome**:
- The pipeline runs `reviewer.py` using the OpenAI API (`gpt-4`) to review `src/main.cpp` and posts the output as a PR comment if a PR exists.

### Final Jenkinsfile
The final `Jenkinsfile` integrates all features:
```groovy
pipeline {
    agent {
        docker {
            image 'ghcr.io/gitusername/gcc-cmake-python:latest'
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
                    userRemoteConfigs: [[url: 'https://github.com/gitusername/jenkinstutorial.git']]
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
                        https://api.github.com/repos/gitusername/jenkinstutorial/pulls?head=gitusername:${env.BRANCH_NAME}&base=main | \
                        jq -r '.[0].number'
                    """, returnStdout: true).trim()
                    if (prNumber != 'null' && prNumber != '') {
                        sh """
                            curl -X POST -H "Authorization: token ${GITHUB_TOKEN}" \
                            -H "Accept: application/vnd.github.v3+json" \
                            https://api.github.com/repos/gitusername/jenkinstutorial/pulls/${prNumber}/comments \
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
```

### Troubleshooting Tips
- **Docker CLI Issues**:
  - Ensured `/var/run/docker.sock` was mounted and Docker Desktop was running.
  - Tested:
    ```powershell
    docker exec -it jenkins-local bash
    docker pull ghcr.io/rikorose/gcc-cmake:latest
    ```
- **Webhook Failures**:
  - Verified ngrok URL and webhook delivery in GitHub.
  - Restarted ngrok if the URL changed:
    ```powershell
    ngrok http 8080
    ```
- **SonarQube Issues**:
  - Ensured `http://host.docker.internal:9000` was accessible.
  - Tested `sonar-scanner` locally:
    ```bash
    docker run -it ghcr.io/rikorose/gcc-cmake:latest bash
    wget https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-4.8.0.2856-linux.zip
    unzip sonar-scanner-cli-4.8.0.2856-linux.zip
    ./sonar-scanner-4.8.0.2856-linux/bin/sonar-scanner --version
    ```
- **LLM Review Issues**:
  - Verified OpenAI API key and `openai` package.
  - Tested `reviewer.py`:
    ```bash
    export OPENAI_API_KEY=sk-1234567890abcdef
    python3 reviewer.py
    ```
  - Ensured `jq` for PR number retrieval.

### Local Testing
- Tested the build in WSL:
  ```bash
  wsl
  cd /mnt/c/Users/username/jenkinstutorial
  rm -rf build
  mkdir build
  cd build
  cmake .. -G "Unix Makefiles"
  make
  ./mathdemo
  ```
- Installed dependencies if needed:
  ```bash
  sudo apt-get update
  sudo apt-get install -y cmake g++ make git
  ```

### Enhancements Suggested
- **Unit Tests**:
  - Add a GoogleTest stage:
    ```groovy
    stage('Test') {
        steps {
            sh './build/run_tests'
        }
    }
    ```
- **Artifact Archiving**:
  - Save `mathdemo`:
    ```groovy
    stage('Archive') {
        steps {
            archiveArtifacts artifacts: 'build/mathdemo', allowEmptyArchive: false
        }
    }
    ```
- **Multibranch Pipeline**:
  - Set up a multibranch pipeline for automatic branch and PR detection.
- **Build Wrapper for SonarQube**:
  - Use `build-wrapper-linux-x86-64` for enhanced C++ analysis:
    ```groovy
    stage('Build') {
        steps {
            sh '''
                mkdir -p build
                cd build
                build-wrapper-linux-x86-64 --out-dir build_wrapper_output cmake .. -G "Unix Makefiles"
                make
            '''
        }
    }
    ```

### Next Steps
- Monitor OpenAI API costs and consider `gpt-4o-mini`.
- Secure SonarQube and Jenkins for production use.
- Explore additional tools like Clang-Tidy or OWASP Dependency-Check.
- Share console output or logs for any issues.

**Date Completed**: July 19, 2025