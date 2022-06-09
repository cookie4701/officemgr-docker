#!/bin/sh

sed -i  "s#ENV_BASEURL#$ENV_BASEURL#" /var/www/html/officemgr/app/Config/App.php
sed -i  "s#ENV_BASEURL#$ENV_BASEURL#" /var/www/html/officemgr/.env
sed -i  "s#ENV_DB_USER#$ENV_DB_USER#" /var/www/html/officemgr/.env
sed -i  "s#ENV_DB_PASS#$ENV_DB_PASS#" /var/www/html/officemgr/.env
sed -i  "s#ENV_DB_HOST#$ENV_DB_HOST#" /var/www/html/officemgr/.env
sed -i  "s#ENV_DB_NAME#$ENV_DB_NAME#" /var/www/html/officemgr/.env
