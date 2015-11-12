#include "mnet/mn_sockaddr.h"
#include "mcoap/mc_code.h"
#include "testbase/print.h"

/** note: does not print CR like other print functions. */
void print_buffer(mc_buffer_t* buffer) {
    uint32_t ibyte;
    for (ibyte = 0; ibyte < buffer->nbytes; ibyte++) {
        printf("%02X", buffer->bytes[ibyte]);
    }
}

void print_option(mc_option_t* option) {
    printf("option: %3d", option->option_num);
    if (option->value.nbytes > 0) {
        printf(", content: 0x");
        print_buffer(&option->value);
        printf("\n");
    }
    else {
        printf(".\n");
    }
}

void print_options(mc_options_list_t* list) {
    uint32_t ioption;

    for (ioption = 0; ioption < list->noptions; ioption++) {
        print_option(list->options + ioption);
    }

}

void print_token(mc_buffer_t* token) {
    uint32_t ibyte;

    printf("token:    ");
    for (ibyte = 0; ibyte < token->nbytes; ibyte++) {
        printf("%x", token->bytes[ibyte]);
    }
    printf("\n");
}

void print_msg(mc_message_t* const msg) {
    if (msg == 0) {
        printf("no response received\n");
    }
    else {
        uint8_t code = mc_message_get_code(msg);

        printf("version:  %d.\n", mc_message_get_version(msg));
        printf("type:     %d.\n", mc_message_get_type(msg));
        printf("msgid:    %d.\n", mc_message_get_message_id(msg));
        print_token(msg->token);
        printf("category: %d.\n", mc_code_get_category(code));
        printf("detail:   %d.\n", mc_code_get_detail(code));
        print_options(msg->options);

        /** @todo check content type. */
        if (msg->payload) printf("%.*s\n", msg->payload->nbytes, msg->payload->bytes);
    }
}