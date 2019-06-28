#include "common.h"
#include "mm.h"
#include "serial.h"
#include "string.h"

#define MAXSIZE_CMD 1024
typedef struct command_line {
	char cmd[MAXSIZE_CMD];
	int pos;
	int count;
	//struct linknode list;
} p_commandline;

struct my_command {
	char *name;
	int (*cmd)(void);
};

int e820(void);
int help(void);

struct my_command cmd_set[] = {
	{ "help", help },	/* help display info about this shell */
	{ "e820", e820 },	/* help display info about this shell */
	{ NULL, NULL }
};

int help()
{
	return 0;
}

int e820()
{
	struct MEMMAP *memmap = (struct MEMMAP *)(MEM_MAP_ADDR);
	char buf[100];
	int pos = 20;
	for (; memmap->type != 0;) {
		sprintf(buf, "base:%8X\t", memmap->base);
		write_string_serial(buf);
		sprintf(buf, "len:%8X\t", memmap->length);
		write_string_serial(buf);
		if (memmap->type == 1) {
			sprintf(buf, "%s\r\n", "free");
		} else {
			sprintf(buf, "%s\r\n", "reserve");
		}
		write_string_serial(buf);
		memmap++;
		pos = pos + 16;
	}
	return 0;
}

int read_cmdline(p_commandline *);
void main_loop(p_commandline *);

int findcmd(char *user_cmdline)
{
	struct my_command *p = cmd_set;
	int ret = 127;

	while (p->name) {
		if (!strncmp(p->name, user_cmdline, strnlen(p->name, 64))) {
			ret = p->cmd();
			return ret;
		}
		p++;
	}
	return ret;
}

void main_loop(p_commandline * cmd)
{
	int ret;

	ret = read_cmdline(cmd);
	if (ret == 127) {
		return;
	}
	//write_string_serial("\r\n");
	if (cmd->count > MAXSIZE_CMD) {
		write_string_serial("command is too long");
		cmd->count = 0;
		cmd->pos = 0;
		cmd->cmd[0] = 0;
		return;
	}
	if (ret == 0) {
		if (cmd->pos > 0) {
			ret = findcmd(cmd->cmd);	//find command as built-in
			if (ret == 127) {
				write_string_serial("Command not found !\n\r");
			}
		}
		write_string_serial("Hypervisor$ ");
	}

	cmd->count = 0;
	cmd->pos = 0;
	cmd->cmd[0] = 0;
}

int read_cmdline(p_commandline * cmd)
{
	int c;

	if ((c = get_serial_fifo()) != '\r') {
		if (c == 0x7f) {	//delete char
			if (cmd->pos > 0) {
				write_serial('\b');
				write_serial(' ');
				write_serial('\b');
				cmd->pos--;
				cmd->count--;
				cmd->cmd[cmd->pos] = '\0';
			}
			return 127;
		}
		cmd->cmd[cmd->pos++] = c;
		cmd->count++;
		write_serial(c);
		return 127;
	}
	cmd->cmd[cmd->pos] = '\0';
	write_string_serial("\r\n");
	return 0;

	while (((c) != '\r') && cmd->count < MAXSIZE_CMD) {
		if (c == 0x1b) {	//move cursor
			c = getchar();
			switch (c) {
			case 0x5b:
				c = getchar();
				switch (c) {
				case 'A':
					break;
				case 'B':
					break;
				case 'C':
					if (cmd->count > cmd->pos) {	//you could go forward
						write_string_serial("\033[C");
						cmd->pos++;
					}
					break;
				case 'D':
					if (cmd->pos > 0) {	//you could go back
						write_string_serial("\033[D");
						cmd->pos--;
					}
					break;
				}
				break;
			}
			continue;
		}
		if (c == 0x7f) {	//delete char
			if (cmd->pos > 0) {
				write_serial('\b');
				write_serial(' ');
				write_serial('\b');
				cmd->pos--;
				cmd->count--;
				cmd->cmd[cmd->pos] = '\0';
			}
			continue;
		}
		if (c == 0x9) {	//tab key
			/*Fix me */
			continue;
		}
		if (cmd->count == cmd->pos) {
			cmd->cmd[cmd->pos++] = c;
			cmd->count++;
			write_serial(c);
		}
		if (cmd->pos < cmd->count) {
			int i = cmd->count;
			while (i > cmd->pos) {
				cmd->cmd[i] = cmd->cmd[i - 1];
				i--;
			}
			if (c == 0x7f) {	//delete char
				if (cmd->pos > 0) {
					write_serial('\b');
					write_serial(' ');
					write_serial('\b');
					cmd->pos--;
					cmd->count--;
				}
				continue;
			}
			cmd->cmd[cmd->pos++] = (char)c;
			cmd->count++;
			for (i = cmd->pos - 1; i < cmd->count; i++) {
				write_serial(cmd->cmd[i]);
			}
			for (i = cmd->count; i > cmd->pos; i--) {
				write_serial('\b');
			}
		}
	}
	if (c == '\r') {
		write_string_serial("\n\r");
	}
	cmd->cmd[cmd->count] = '\0';
	return 0;
}

p_commandline cmd = {
	.count = 0,
	.pos = 0,
	.cmd[0] = '\0',
};

void serial_console()
{
	main_loop(&cmd);
	return;
}
