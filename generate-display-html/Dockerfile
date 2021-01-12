FROM node:15-alpine
WORKDIR /usr/src/app

RUN npm install mqtt --save

COPY generate-display-html.js .

CMD [ "node", "generate-display-html.js"]

EXPOSE 80/tcp

