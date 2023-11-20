FROM jenkins/inbound-agent:3192.v713e3b_039fb_e-1-jdk17

USER root

ENV UNATTENDED 1

RUN apt update && \
    apt install -y \
        clang \
        cmake \
        wget

WORKDIR /tmp

RUN wget https://download.docker.com/linux/debian/dists/bookworm/pool/stable/amd64/docker-ce-cli_24.0.7-1~debian.12~bookworm_amd64.deb
RUN wget https://download.docker.com/linux/debian/dists/bookworm/pool/stable/amd64/docker-buildx-plugin_0.11.2-1~debian.12~bookworm_amd64.deb
RUN dpkg -i ./*.deb

WORKDIR /home/jenkins

USER jenkins

RUN mkdir /home/jenkins/.docker_secret
RUN touch /home/jenkins/.docker_secret/.dockerconfigjson
RUN mkdir /home/jenkins/.docker
RUN ln -s /home/jenkins/.docker_secret/.dockerconfigjson /home/jenkins/.docker/config.json

