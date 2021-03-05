# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
idf_component_register(SRCS "HTTP_REQUEST_API.c" "WIFI_API.c" "main.c"
                    INCLUDE_DIRS ".")