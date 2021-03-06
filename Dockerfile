ARG ALPINE_VERSION=3.15
FROM alpine:${ALPINE_VERSION}
# Original Dockerfile from Time de Pater (code@trafx.nl)

LABEL Maintainer="PaKu <pascal@pkit.eu>"
LABEL Description="Lightweight container with Nginx 1.20 & PHP 8.0 based on Alpine Linux. Includes officemgr"
# Setup document root
WORKDIR /var/www/html

# Install packages and remove default server definition
RUN apk add --no-cache \
  curl \
  nginx \
  php8 \
  php8-ctype \
  php8-curl \
  php8-dom \
  php8-fpm \
  php8-gd \
  php8-intl \
  php8-mbstring \
  php8-mysqli \
  php8-opcache \
  php8-openssl \
  php8-phar \
  php8-session \
  php8-xml \
  php8-xmlreader \
  php8-xmlwriter \
  php8-zlib \
  php8-tokenizer \
  supervisor \
  git \
  composer \
  sed

# Create symlink so programs depending on `php` still function
RUN ln -s /usr/bin/php8 /usr/bin/php

# Configure nginx
COPY config/nginx.conf /etc/nginx/nginx.conf

# Configure PHP-FPM
COPY config/fpm-pool.conf /etc/php8/php-fpm.d/www.conf
COPY config/php.ini /etc/php8/conf.d/custom.ini

# Configure supervisord
COPY config/supervisord.conf /etc/supervisor/conf.d/supervisord.conf

RUN mkdir /utils
COPY scripts/replace_env.sh /utils/replace_env.sh

# Make sure files/folders needed by the processes are accessable when they run under the nobody user
RUN chown -R nobody.nobody /var/www/html /run /var/lib/nginx /var/log/nginx


# Switch to use a non-root user from here on
USER nobody

# Add application
RUN git clone https://github.com/cookie4701/officemgr.git officemgr  && \
    cd officemgr && \
    composer install && \
    chmod 777 -R /var/www/html/officemgr/writable && \
    mv /var/www/html/officemgr/env /var/www/html/officemgr/.env && \
    chown -R nobody.nobody /var/www/html/officemgr


USER root
RUN chmod ugo+x /utils/replace_env.sh
RUN mkdir /var/www/html/officemgr/writable/debugbar && \
  mkdir /var/www/html/officemgr/writable/cache && \
  mkdir /var/www/html/officemgr/writable/logs && \
  mkdir /var/www/html/officemgr/writable/uploads
RUN chown -R nobody.nobody /var/www/html/officemgr/writable

USER nobody

# Expose the port nginx is reachable on
EXPOSE 8080

# Let supervisord start nginx & php-fpm
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]

# Configure a healthcheck to validate that everything is up&running
HEALTHCHECK --timeout=10s CMD curl --silent --fail http://127.0.0.1:8080/fpm-ping
