FROM node:21-alpine

COPY app /autoupdater

WORKDIR /autoupdater

RUN npm install

ENV DEBUG=myapp:*

CMD ["npm", "start"]
