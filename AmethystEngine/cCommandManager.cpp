#include "cCommandManager.hpp"



#pragma region SINGLETION
cCommandManager cCommandManager::stonCmdMngr;
cCommandManager* cCommandManager::GetCommandManager() { return &(cCommandManager::stonCmdMngr); }
cCommandManager::cCommandManager() { std::cout << "Command Manager Created" << std::endl; pMediator = nullptr; cmdMove = nullptr; cmdRotate = nullptr; }
#pragma endregion



#pragma region MEDIATOR_COMMUNICATIONS
void cCommandManager::setMediatorPointer(iMediatorInterface* pMediator) {
	this->pMediator = pMediator;
	return;
}



void cCommandManager::ConvertToLower(std::string cmdstr) {
	std::string s;
	if (!cmdstr.empty()) {
		if (cmdstr.front() == '`')
			cmdstr = cmdstr.substr(1, cmdstr.back());
		for (char c : cmdstr)
			s += std::tolower(c);
		cmdstr = s;
	}
}



void cCommandManager::CreateCommands() {
	/*m_str_to_enum[vec_move_to_cmd_strs] = MOVE;
	m_str_to_enum[vec_rotate_to_cmd_strs] = ROTATE;
	cmdMove = new cMove();
	cmdRotate = new cRotate();*/
}


iCommand* cCommandManager::CreateAnimScript() {
	iCommand* cmdscrp = nullptr;
	return cmdscrp;
}
iCommand* cCommandManager::CreateCommandGroup() {
	iCommand* cmdgrp = nullptr;
	return cmdgrp;
}

iCommand* cCommandManager::CreateCommand(cmdType cmd_type, sData data) {
	iCommand* cmd = nullptr;
	switch (cmd_type) {
	//case MOVE:
	//	cmd = new cMove();
	//	cmd->Init(data);
	//	cmdMove = cmd;
	//	break;

	//case ROTATE:
	//	cmd = new cRotate();
	//	cmd->Init(data);
	//	cmdRotate = cmd;
	//	break;

	default:
		// user can no enter command right...
		break;
	}
	return cmd;
}



void cCommandManager::IssueCommand(std::string cmd) {
	sNVData data;
	_cmdstr = cmd;

	std::string temp;

	// parse cmd for data population
	std::vector<std::string> vdata;
	
	bool targetEntry = false;
	for (char c : cmd) {
		if (c == '.') {
			vdata.push_back(temp);
			temp = "";
		}
		else if (c == '"') {
			if (vdata.size() == 0)
				vdata.push_back("");
			if(targetEntry == false)
				targetEntry = true;
			else {
				if (vdata.size() == 0)
					vdata.push_back("");
				targetEntry = false;
				vdata.push_back(temp);
				temp = "";
			}
		}
		else if (c == ' ') {
			if (vdata.size() == 0)
				vdata.push_back("");
			if (!temp.empty())
				vdata.push_back(temp);
			temp = "";
		}
		else {
			temp += c;
		}
	}

	if (!temp.empty()) {
		vdata.push_back(temp);
		temp = "";
	}

	// detection for specific commands
	// i.e. save, control, etc.

	// SPECIAL SINGLE TYPE COMMANDS
	if (vdata.size() == 1) {
		ConvertToLower(vdata[0]);
		data.cmd = vdata[0];
	}

	else if (vdata[1] == "savefiles") {
		ConvertToLower(vdata[1]);
		data.cmd = vdata[1];
		data.target = "M";
		data._models_filename = vdata[2];
		data._lights_filename = vdata[3];
		data._complex_filename = vdata[4];
	}

	else if (vdata[1] == "control") {
		ConvertToLower(vdata[0]);
		ConvertToLower(vdata[1]);
		data.subsystem = vdata[0];
		data.cmd = vdata[1];
	}

	// cmd: data [show] [hide] [toggle]
	else if (vdata[1] == "data") {
		ConvertToLower(vdata[1]);
		ConvertToLower(vdata[2]);
		data.subsystem = "";
		data.cmd = vdata[1];
		data.state = vdata[2];
	}

	else if (vdata[1] == "getposition") {
		ConvertToLower(vdata[1]);
		data.subsystem = "";
		data.cmd = vdata[1];
		data.target = vdata[2];
	}

	else {
		if (vdata[1] == "create") {
			if (vdata[0] == "model") {
				ConvertToLower(vdata[0]);
				ConvertToLower(vdata[1]);

				data.subsystem = vdata[0];
				data.cmd = vdata[1];
				data.mesh = vdata[2];
				data.name = vdata[3];
			}
			else if (vdata[0] == "light") {
				ConvertToLower(vdata[0]);
				ConvertToLower(vdata[1]);

				data.subsystem = vdata[0];
				data.cmd = vdata[1];
				data.name = vdata[2];
			}
		}
		else if (vdata.size() == 3) {
			ConvertToLower(vdata[0]);
			ConvertToLower(vdata[1]);
			ConvertToLower(vdata[2]);

			if (vdata[0] != "")
				data.subsystem = vdata[0];
			data.cmd = vdata[1];
			data.state = vdata[2];
		}
		else if (vdata[0] != "") {
			if (vdata[0] != "") {
				ConvertToLower(vdata[0]);

				data.subsystem = vdata[0];
			}

			ConvertToLower(vdata[1]);
			data.cmd = vdata[1];
			data.target = vdata[2];

			if (vdata.size() == 4) {
				ConvertToLower(vdata[3]);
				data.state = vdata[3];
			}
		}
		else {
			ConvertToLower(vdata[0]);
			ConvertToLower(vdata[1]);
			ConvertToLower(vdata[3]);

			data.cmd = vdata[1];
			data.state = vdata[3];
			data.target = vdata[2];
			data.subsystem = vdata[0];
		}
	}
	this->RecieveMessage(data);
}


void cCommandManager::IssueCommand(sData& data) {
	this->RecieveMessage(data);
}


sNVData cCommandManager::RecieveMessage(sNVData message) {
	sNVData response;
	response.status = "OK";

	if(message.status == "OK") { }
	else if(message.status == "NOK") { }

	// DEVCON PRINT OUT
	// This should probably be changed and filled with more information
	if (pMediator->RecieveMessage(message).status == "NOK")
		_cmd_result = "UNKNOWN COMMAND: " + _cmdstr;
	else
		_cmd_result = "COMMAND ACCEPTED: " + _cmdstr;

	_cmdstr = "";
	return response;
}



sData cCommandManager::RecieveMessage(sData& data){
	switch (pMediator->RecieveMessage(data).getResult()) {
		case OK:
			// command execution successfull...
			break;

		case UNKNOWN_SUBSYSTEM:
			// unknown subsystem specified for command execution, command execution failed...
			break;

		case UNKNOWN_COMMAND:
			// unknown command specified for command execution, command execution failed...
			break;

		case INVALID_COMMAND:
			// invalid command specified for specified subsystem, command execution failed...
			break;

		case NOK:
			// unknown error caught while executing command, command execution failed...
			break;

		default:
			data.setResult(NOK);
			// Feeney style oh no error!
			std::cout << "OH NO!!!!!!!!" << std::endl;
			// it should never reach this case at all, this is a catastrophic error...
			break;
	}
	return data;
}
#pragma endregion
