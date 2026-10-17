 server {
    server_name www.aot.ru aot.ru;
    #server_name 63.250.59.227;
    #server_name localhost;
    
    root /home/sokirko/RML/Source/www/wwwroot;
    index  index.html;
    client_max_body_size 5m;
    access_log /var/log/nginx/aot-access.log;
    error_log  /var/log/nginx/aot-error.log;


    location /synan-daemon/ {
        proxy_pass http://127.0.0.1:17017/;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }

    location /seman-daemon/ {
        proxy_pass http://127.0.0.1:17018/;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }



    location /cgi-bin/ { 
      # Disable gzip (it makes scripts feel slower since they have to complete
      # before getting gzipped)
      gzip off;

      # Set the root to /usr/lib (inside this location this means that we are
      # giving access to the files under /usr/lib/cgi-bin)
      root	/home/sokirko/RML/Source/www;

      # Fastcgi socket
      fastcgi_pass  unix:/var/run/fcgiwrap.socket;

      # Fastcgi parameters, include the standard ones
      include /etc/nginx/fastcgi_params;

    # Adjust non standard parameters (SCRIPT_FILENAME)
    #fastcgi_param SCRIPT_FILENAME  /usr/lib$fastcgi_script_name;
  } 

    # ATTENTTION! You cannot switch this site to https, because the used javascript would issue an error 
    #  morph.js:59 Mixed Content: The page at 'https://aot.ru/demo/morph.html' was loaded over HTTPS, but requested an insecu>
    #  morph_request @ morph.js:59
    # so you have to implement https in all web servers (ddc, SynanDaemon, SemanDaemon)




    listen 443 ssl; # managed by Certbot
    ssl_certificate /etc/letsencrypt/live/aot.ru/fullchain.pem; # managed by Certbot
    ssl_certificate_key /etc/letsencrypt/live/aot.ru/privkey.pem; # managed by Certbot
    include /etc/letsencrypt/options-ssl-nginx.conf; # managed by Certbot
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem; # managed by Certbot


}

#server {
#  server_name www.aot.ru
#  listen 443 ssl;
#  return 404;
#}
 server {
    if ($host = www.aot.ru) {
        return 301 https://$host$request_uri;
    } # managed by Certbot


    if ($host = aot.ru) {
        return 301 https://$host$request_uri;
    } # managed by Certbot


    server_name www.aot.ru aot.ru;


    listen 80 ;
    return 404; # managed by Certbot




}
