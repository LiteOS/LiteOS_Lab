# -*- coding: utf-8 -*-

import sys,os
os.environ['PATH'] += ";"
os.environ['PATH'] += os.getcwd()
import ctypes
import json
import shutil
from PyQt5.QtWidgets import QMessageBox
from PyQt5 import QtWidgets
from ota_package_tool_form import Ui_Form
from Crypto.Signature import pkcs1_15
import Crypto.Hash
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA256

hdiff_result_dict = {0: "HDIFF_SUCCESS",
                     1: "HDIFF_OPTIONS_ERROR",
                     2: "HDIFF_OPENREAD_ERROR",
                     3: "HDIFF_OPENWRITE_ERROR",
                     4: "HDIFF_FILECLOSE_ERROR",
                     5: "HDIFF_MEM_ERROR",
                     6: "HDIFF_DIFF_ERROR",
                     7: "HDIFF_PATCH_ERROR",
                     8: "HDIFF_RESAVE_FILEREAD_ERROR",
                     9: "HDIFF_RESAVE_DIFFINFO_ERROR",
                     10: "HDIFF_RESAVE_COMPRESSTYPE_ERROR",
                     11: "HDIFF_RESAVE_ERROR",
                     12: "HDIFF_RESAVE_CHECKSUMTYPE_ERROR",
                     13: "HDIFF_PATHTYPE_ERROR",
                     14: "HDIFF_TEMPPATH_ERROR",
                     15: "HDIFF_DELETEPATH_ERROR",
                     16: "HDIFF_RENAMEPATH_ERROR",
                     101: "DIRDIFF_DIFF_ERROR",
                     102: "DIRDIFF_PATCH_ERROR",
                     103: "MANIFEST_CREATE_ERROR",
                     104: "MANIFEST_TEST_ERROR"}

class Ota_Package_Tool_Form(QtWidgets.QWidget, Ui_Form):
    def __init__(self):
        super(Ota_Package_Tool_Form, self).__init__()
        self.setupUi(self)
        #self.set_test_data()

    def set_test_data(self):
        current_dir = os.getcwd()
        self.sota_json_file_line_edit.setText(current_dir + os.sep + "UpgradeDesc.json")
        self.oldfile_line_edit.setText(current_dir + os.sep + "1.exe")
        self.newfile_line_edit.setText(current_dir + os.sep + "2.exe")
        self.private_key_line_edit.setText(current_dir + os.sep + "PEM" + os.sep + "private.pem")

    def sota_radio_toggled(self,flag):
        if self.sota_radio_button.isChecked() == True :
            self.import_sota_json_file_button.setDisabled(False)
            self.sota_json_file_line_edit.setDisabled(False)
        else:
            self.import_sota_json_file_button.setDisabled(True)
            self.sota_json_file_line_edit.setDisabled(True)

    def sign_radio_toggled(self,flag):
        if self.sign_radio_button.isChecked() == True :
            self.import_key_button.setDisabled(False)
            self.private_key_line_edit.setDisabled(False)
        else:
            self.import_key_button.setDisabled(True)
            self.private_key_line_edit.setDisabled(True)

    def diff_radio_toggled(self,flag):
        if self.diff_radio_button.isChecked() == True :
            self.import_oldfile_button.setDisabled(False)
            self.oldfile_line_edit.setDisabled(False)
        else:
            self.import_oldfile_button.setDisabled(True)
            self.oldfile_line_edit.setDisabled(True)

    def write_data_to_file(self,file,data,mode):
        fd = open(file, mode)
        fd.write(data)
        fd.close()

    def get_filename_from_json_file(self):
        with open(self.sota_json_file_line_edit.text(), 'r') as load_f:
            load_dict = json.load(load_f)
            ret = load_dict.get("fileName")
        load_f.close()
        return ret

    def generate_key_pares(self):
        try:
            dir_path = QtWidgets.QFileDialog.getExistingDirectory(self, "选择保存公私钥目录", os.getcwd())
            if dir_path:
                key = RSA.generate(2048)
                private_key = key.export_key()
                public_key = key.publickey().export_key()
                private_key_file = dir_path + os.sep + "private.pem"
                public_key_file = dir_path + os.sep + "public.pem"
                self.write_data_to_file(private_key_file, private_key, "wb")
                self.write_data_to_file(public_key_file, public_key, "wb")
        except (IOError,Exception) as ex:
            error_message = "生成公钥public.pem和私钥private.pem失败: %s!" % (ex)
            QMessageBox.warning(self, "warnning", error_message)
        except:
            QMessageBox.warning(self, "warnning", "生成公钥public.pem和私钥private.pem失败")
        else:
            QMessageBox.information(self, "info", "生成公钥public.pem和私钥private.pem成功")

    def import_oldfile(self):
        fileName, ok = QtWidgets.QFileDialog.getOpenFileName(self, "导入旧版本", os.getcwd(),
                                                                   "All Files(*)")
        if ok:
            self.oldfile_line_edit.setText(fileName)

    def import_newfile(self):
        fileName, ok = QtWidgets.QFileDialog.getOpenFileName(self, "导入新版本", os.getcwd(),
                                                                   "All Files(*)")
        if ok:
            self.newfile_line_edit.setText(fileName)

    def import_json_file(self):
        fileName, ok = QtWidgets.QFileDialog.getOpenFileName(self, "导入软件升级描述文件", os.getcwd(),
                                                                   "All Files(*.json)")
        if ok:
            self.sota_json_file_line_edit.setText(fileName)

    def import_private_key(self):
        fileName, ok = QtWidgets.QFileDialog.getOpenFileName(self, "导入签名用私钥", os.getcwd(),
                                                                   "All Files(*.pem)")
        if ok:
            self.private_key_line_edit.setText(fileName)

    def generate_diff_file(self,out_diff_file):
            hdiff_lib = ctypes.cdll.LoadLibrary("libhdiffz.dll")
            program_name = ctypes.create_string_buffer("ota_packeage_tool".encode("utf-8"))
            hdiff_option = ctypes.create_string_buffer("-c-lzma-9-4096".encode("utf-8"))
            new_file = ctypes.create_string_buffer(self.newfile_line_edit.text().encode("utf-8"))
            diff_file = ctypes.create_string_buffer(out_diff_file.encode("utf-8"))
            if self.diff_radio_button.isChecked():
                old_file = ctypes.create_string_buffer(self.oldfile_line_edit.text().encode("utf-8"))
            else:
               old_file = ctypes.create_string_buffer("".encode("utf-8"))
            ret = hdiff_lib.hdiff_cmd_line_ota(ctypes.byref(program_name), ctypes.byref(hdiff_option),
                                        ctypes.byref(old_file), ctypes.byref(new_file),
                                        ctypes.byref(diff_file))
            #ctypes.cdll.FreeLibrary(hdiff_lib)
            return ret

    def generate_signature(self,diff_file, list):
        hash_object = SHA256.new()
        for i in range(len(list)):
            hash_object.update(list[i])
        with open(diff_file, 'rb') as fd:
            chunk = 0
            while chunk != b'':
                chunk = fd.read(1024)
                hash_object.update(chunk)
        fd.close()
        key = RSA.import_key(open(self.private_key_line_edit.text()).read())
        signature = pkcs1_15.new(key).sign(hash_object)
        list.insert(0,signature)

    def write_list_to_file(self,diff_file, list):
        [file_dir,file_name]=os.path.split(diff_file)
        blk = 1024
        tmp_file = os.path.join(file_dir, "tmp")
        fd_read = open(diff_file, "rb")
        fd_write = open(tmp_file, "wb")
        for i in range(len(list)):
            fd_write.write(list[i])
        while True:
            strb = fd_read.read(blk)
            fd_write.write(strb)
            if strb == b"":
                break
        fd_write.close()
        fd_read.close()
        os.remove(diff_file)
        os.rename(tmp_file,diff_file)

    def add_ota_info_signature_to_diff_file(self,diff_file):
        if(self.diff_radio_button.isChecked()):
            ota_info_patch_type = bytes([1])
        else:
           ota_info_patch_type = bytes([0])
        ota_info_reserved = bytes(31)
        list = []
        list.append(ota_info_patch_type)
        list.append(ota_info_reserved)
        if(self.sign_radio_button.isChecked()):
            self.generate_signature(diff_file,list)
        self.write_list_to_file(diff_file,list)

    def generate_sota_package(self):
        try:
            sota_package_path,ok = QtWidgets.QFileDialog.getSaveFileName(self, "保存升级包", os.getcwd(),
                                                "All Files(*.zip)")

            if ok:
                [sota_package_dir,sota_package_name] = os.path.split(sota_package_path)
                sota_package_zip_data_dir = sota_package_dir + os.sep + sota_package_name[:-4]
                upgrade_desc_dir = sota_package_zip_data_dir + os.sep + "DM" + os.sep + "linux"
                diff_file_dir = sota_package_zip_data_dir + os.sep + "linux"
            else:
                return

            #clear sota zip data dir
            if os.path.exists(sota_package_zip_data_dir):
                shutil.rmtree(sota_package_zip_data_dir)
            os.makedirs(upgrade_desc_dir)
            os.makedirs(diff_file_dir)
            diff_file_name = self.get_filename_from_json_file()
            if(diff_file_name == None) :
                error_message = "从升级描述文件%s中解析fileName失败，请检查升级描述文件!" % (self.sota_json_file_line_edit.text())
                QMessageBox.warning(self, "warnning", error_message)
                return
            upgrade_desc_file = upgrade_desc_dir + os.sep + "UpgradeDesc.json"
            shutil.copy(self.sota_json_file_line_edit.text(), upgrade_desc_file)
            diff_file = diff_file_dir + os.sep + diff_file_name
            ret = self.generate_diff_file(diff_file)
            if ret != 0 :
                error_message = "差分文件生成失败,错误码 %s!" % (hdiff_result_dict[ret])
                QMessageBox.warning(self, "warnning", error_message)
                return
            self.add_ota_info_signature_to_diff_file(diff_file)

            shutil.make_archive(sota_package_name[:-4], "zip", sota_package_zip_data_dir)

        except json.decoder.JSONDecodeError as ex:
            error_message = "生成软件升级包失败,升级描述文件格式不正确: %s!" % (ex)
            QMessageBox.warning(self, "warnning", error_message)
        except Exception as ex:
            error_message = "生成软件升级包失败, %s!" % (ex)
            QMessageBox.warning(self, "warnning", error_message)
        except:
            QMessageBox.warning(self, "warnning", "生成软件升级包失败")
        else :
            QMessageBox.information(self, "info", "生成软件升级包：" + sota_package_path)
            if os.path.exists(sota_package_zip_data_dir):
                shutil.rmtree(sota_package_zip_data_dir)


    def generate_fota_package(self):
        try:
            fota_package_path,ok = QtWidgets.QFileDialog.getSaveFileName(self, "保存升级包", os.getcwd(),
                                                "All Files(*.zip)")
            if ok:
                [fota_package_dir, fota_package_name] = os.path.split(fota_package_path)
                diff_file_dir = fota_package_dir + os.sep + fota_package_name[:-4]
            else:
                return

            #clear fota zip data dir
            if os.path.exists(diff_file_dir):
                shutil.rmtree(diff_file_dir)
            os.makedirs(diff_file_dir)
            diff_file = diff_file_dir + os.sep + fota_package_name[:-4] + ".bin"
            ret = self.generate_diff_file(diff_file)
            if ret != 0 :
                error_message = "差分文件生成失败,错误码 %s!" % (hdiff_result_dict[ret])
                QMessageBox.warning(self, "warnning", error_message)
                return
            self.add_ota_info_signature_to_diff_file(diff_file)

            shutil.make_archive(fota_package_name[:-4], "zip", diff_file_dir)

        except Exception as ex:
            error_message = "生成固件升级包失败, %s!" % (ex)
            QMessageBox.warning(self, "warnning", error_message)
        except:
            QMessageBox.warning(self, "warnning", "生成固件升级包失败")
        else:
            QMessageBox.information(self, "info", "生成固件升级包：" + fota_package_path)
            if os.path.exists(diff_file_dir):
                shutil.rmtree(diff_file_dir)

    def generate_package(self):
        if (self.sota_radio_button.isChecked()):
            if (os.path.exists(self.sota_json_file_line_edit.text())  == False) :
                error_message = "软件升级描述文件不存在！%s!" % (self.sota_json_file_line_edit.text())
                QMessageBox.warning(self, "warnning", error_message)
                return
        if (self.diff_radio_button.isChecked()):
            if (os.path.exists(self.oldfile_line_edit.text())  == False or \
                    os.path.exists(self.newfile_line_edit.text()) == False ) :
                error_message = "旧版本或新版本文件不存在！%s\n %s!" % (self.oldfile_line_edit.text(),
                                                             self.newfile_line_edit.text())
                QMessageBox.warning(self, "warnning", error_message)
                return
        else:
            if (os.path.exists(self.newfile_line_edit.text()) == False) :
                error_message = "新版本文件不存在！%s!" % (self.newfile_line_edit.text())
                QMessageBox.warning(self, "warnning", error_message)
                return
        if (self.sign_radio_button.isChecked()):
            if (os.path.exists(self.private_key_line_edit.text())  == False) :
                error_message = "签名用私钥文件不存在！%s!" % (self.private_key_line_edit.text())
                QMessageBox.warning(self, "warnning", error_message)
                return
        if (self.sota_radio_button.isChecked()):
            self.generate_sota_package()
        else:
            self.generate_fota_package()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    ota_form = Ota_Package_Tool_Form()
    ota_form.show()
    sys.exit(app.exec_())
