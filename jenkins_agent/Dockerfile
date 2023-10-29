FROM jenkins/inbound-agent:3181.v78543a_987053-1

USER root

ENV UNATTENDED 1

RUN apt update && \
    apt install -y \
        clang \
        cmake \
        wget

RUN wget https://dl.k8s.io/release/v1.28.3/bin/linux/amd64/kubectl -P /tmp
RUN wget https://github.com/vmware-tanzu/buildkit-cli-for-kubectl/releases/download/v0.1.6/linux-v0.1.6.tgz -P /tmp

RUN install -o root -g root -m 0755 /tmp/kubectl /usr/local/bin/kubectl
RUN tar xzf /tmp/linux-v0.1.6.tgz -C /usr/local/bin

USER jenkins

