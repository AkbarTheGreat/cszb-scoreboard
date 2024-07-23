FROM node:21-alpine

COPY app /autoupdater

WORKDIR /autoupdater

RUN npm install
RUN npm run build

ENV DEBUG=myapp:*

CMD ["npm", "start"]
# npm run dev to debug
