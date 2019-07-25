#include <fstream>

#include "ivp_iw.h"

#include <iostream>
namespace IVP {

    //Eigen::VectorXd matRound(Eigen::VectorXd vec) {
    //    for (int i = 0; i < vec.rows(); ++i)
    //        vec(i) = round(vec(i));
    //    return vec;
    //}


    //Eigen::MatrixXd matRound(Eigen::MatrixXd mat) {
    //    for (int i = 0; i < mat.rows(); ++i)
    //        for (int j = 0; j < mat.cols(); ++j)
    //            mat(i, j) = round(mat(i, j));
    //    return mat;
    //}


    void IVPBase::loadCSV(std::string filename) {
        this->screenList_.clear();
        this->bind2gray.clear();

        std::ifstream fin(filename, std::ios::in);
        std::string line;

        std::getline(fin, line);
        sscanf_s(line.c_str() + 4, "%d,%d", &(this->initSize_), &(this->refreshIdx_));
        --this->refreshIdx_;
        
        int tmpIdx = 1;
        int screenIdx, bandIdx, bindIdx, grayVal, rReg, gReg, bReg;
        std::map<int, std::vector<double>> screen;
        while (std::getline(fin, line)) {
            sscanf_s(line.c_str(), "%d-%d-%d-%d,%x,%x,%x", &screenIdx, &bandIdx, &bindIdx, &grayVal, &rReg, &gReg, &bReg);
            if (screenIdx != tmpIdx) {
                this->screenList_.push_back(screen);

                if (screenIdx > this->initSize_)
                    break;
                tmpIdx = screenIdx;
                screen.clear();
            }

            screen[bindIdx] = { static_cast<double>(rReg), static_cast<double>(gReg), static_cast<double>(bReg) };
            this->bind2gray[bindIdx] = grayVal;
        }

        fin.close();

        if (this->maxSize_ < this->initSize_)
            this->maxSize_ = this->initSize_;

        this->buildMatrix();
    }


    void IVPBase::refreshScreen(const std::map<int, std::vector<double>>& screen) {
        if (static_cast<int>(this->screenList_.size()) < this->maxSize_) {
            this->screenList_.push_back(screen);
            this->refreshIdx_ = this->screenList_.size() - 1;
        }
        else {
            this->refreshIdx_ = (this->refreshIdx_ + 1) % this->screenList_.size();
            this->screenList_[this->refreshIdx_] = screen;
        }

        this->buildMatrix();
    }


    void IWAlgo::iwSetPara(double lamda) {
        this->initLamda_ = lamda;
        this->initLamdaList();
    }


    void IWAlgo::iwAutoPara(std::vector<double> lamdaRange) {
        if (lamdaRange.size() == 3)
            this->lamdaRange_ = lamdaRange;
        this->initLamdaList();

        // 逐个筛选bind的超参数(注意, 最后一个bind无需筛选lamda)
        std::vector<CalcW> W_List, W_ListBack;
        this->initInnerW(W_List);
        for (int bindIdx = 1; bindIdx < static_cast<int>(this->screenList_[0].size()); ++bindIdx) {
            std::vector<CalcMat> Mat_List, Mat_ListNext;
            this->initInnerMat(Mat_List, bindIdx);                // 更新W
            this->initInnerMat(Mat_ListNext, bindIdx + 1);        // 计算loss

            W_ListBack = W_List;
            std::map<double, double> lamda2loss;
            for (double lamda = this->lamdaRange_[0]; lamda < this->lamdaRange_[1]; lamda += this->lamdaRange_[2]) {
                if (lamda <= -1) continue;
                
                this->updateInnerW(W_ListBack, Mat_List, bindIdx, lamda);

                double totalLoss = this->calcTotalLoss(W_ListBack, Mat_ListNext, bindIdx + 1);
                lamda2loss[lamda] = totalLoss;

                W_ListBack = W_List;
            }

            double minLamda = this->findLamdaByLoss(lamda2loss);
            this->updateInnerW(W_List, Mat_List, bindIdx, minLamda);
            this->lamdaList_[bindIdx - 1] = minLamda;

            std::cout << minLamda << std::endl;
        }
    }


    double IWAlgo::findLamdaByLoss(std::map<double, double>& lamda2loss) {
        double minLamda = lamda2loss.begin()->first;
        double minLoss = lamda2loss.begin()->second;

        for (auto& iter : lamda2loss) {
            if (iter.second < minLoss) {
                minLamda = iter.first;
                minLoss = iter.second;
            }
        }

        return minLamda;
    }


    double IWAlgo::calcTotalLoss(std::vector<CalcW>& W_List, std::vector<CalcMat>& Mat_List, int bindIdx) {
        double totalLoss = 0;

        for (int screenIdx = 0; screenIdx < static_cast<int>(W_List.size()); ++screenIdx) {
            CalcW& tmpW_old = W_List[screenIdx];
            CalcMat& tmpMat = Mat_List[screenIdx];

            double loss = this->calcLoss(tmpW_old, tmpMat, bindIdx);
            totalLoss += loss;
        }

        return totalLoss / W_List.size();
    }


    double IWAlgo::calcLoss(CalcW& W_old, CalcMat& Mat, int bindIdx) {
        Eigen::MatrixXd& A = Mat.A_;
        Eigen::VectorXd& Wold = (this->bind2gray[bindIdx] == 255) ? W_old.W255_ : W_old.Wold_;

        Eigen::VectorXd D_pred = A * Wold;
        double loss = (Mat.D_ - D_pred).norm();

        return loss;
    }

    
    void IWAlgo::updateInnerW(std::vector<CalcW>& W_List, std::vector<CalcMat>& Mat_List, int bindIdx, double lamda) {
        for (int screenIdx = 0; screenIdx < static_cast<int>(W_List.size()); ++screenIdx) {
            CalcW& tmpW_old = W_List[screenIdx];
            CalcMat& tmpMat = Mat_List[screenIdx];
            Eigen::VectorXd tmpW = this->calcW(tmpW_old, tmpMat, bindIdx, lamda);

            this->updateW(tmpW_old, tmpW, bindIdx);
        }
    }


    void IWAlgo::updateW(CalcW& W_old, Eigen::VectorXd& W, int bindIdx) {
        if (this->bind2gray[bindIdx] == 255)
            W_old.W255_ = W;

        W_old.Wold_ = W;
    }


    Eigen::VectorXd IWAlgo::calcW(CalcW& W_old, CalcMat& Mat, int bindIdx, double lamda) {
        const double epsilon = 1.e-9;                     // 计算精度
        Eigen::MatrixXd& A = Mat.A_;
        Eigen::VectorXd& D = Mat.D_;
        Eigen::MatrixXd& E = Mat.E_;
        Eigen::VectorXd& Wold = (this->bind2gray[bindIdx] == 255) ? W_old.W255_ : W_old.Wold_;
        Eigen::VectorXd beta = (epsilon * E + A * A.transpose()).inverse() * (A * Wold - (1 + lamda) * D);
        Eigen::VectorXd W = (Wold - A.transpose() * beta) / (1 + lamda);

        return W;
    }


    void IWAlgo::initInnerMat(std::vector<CalcMat>& Mat_List, int bindIdx) {
        for (int screenIdx = 0; screenIdx < static_cast<int>(this->screenList_.size()); ++screenIdx) {

            Eigen::MatrixXd tmpA(4, this->screenList_.size() - 1);
            Eigen::VectorXd tmpD(4);
            Eigen::MatrixXd tmpE = Eigen::MatrixXd::Identity(4, 4);

            int cnt = 0;
            for (int screenIdx2 = 0; screenIdx2 < static_cast<int>(this->screenList_.size()); ++screenIdx2) {
                if (screenIdx2 != screenIdx) {
                    tmpA(0, cnt) = this->screenList_[screenIdx2][bindIdx][0];
                    tmpA(1, cnt) = this->screenList_[screenIdx2][bindIdx][1];
                    tmpA(2, cnt) = this->screenList_[screenIdx2][bindIdx][2];
                    tmpA(3, cnt) = 1;
                    ++cnt;
                }
                else {
                    tmpD(0) = this->screenList_[screenIdx2][bindIdx][0];
                    tmpD(1) = this->screenList_[screenIdx2][bindIdx][1];
                    tmpD(2) = this->screenList_[screenIdx2][bindIdx][2];
                    tmpD(3) = 1;
                }
            }

            Mat_List.push_back(CalcMat{ tmpA, tmpD, tmpE });
        }
    }


    void IWAlgo::initInnerW(std::vector<CalcW>& W_List) {
        int size = this->screenList_.size() - 1;
        for (int screenIdx = 0; screenIdx < static_cast<int>(this->screenList_.size()); ++screenIdx) {
            CalcW W;
            this->initW(W, size);
            W_List.push_back(W);
        }
    }


    void IWAlgo::initW(CalcW& W, int size) {
        W.W255_ = Eigen::VectorXd::Ones(size) / size;
        W.Wold_ = Eigen::VectorXd::Ones(size) / size;
    }


    void IWAlgo::initLamdaList() {
        this->lamdaList_.clear();

        for (int bindIdx = 1; bindIdx < static_cast<int>(this->screenList_[0].size() + 1); ++bindIdx)
            this->lamdaList_.push_back(this->initLamda_);
    }


    void IWAlgo::buildMatrix() {
        for (int bindIdx = 1; bindIdx < static_cast<int>(this->screenList_[0].size() + 1); ++bindIdx) {
            Eigen::MatrixXd tmpA(4, this->screenList_.size());
            Eigen::VectorXd tmpD(4);
            Eigen::MatrixXd tmpE = Eigen::MatrixXd::Identity(4, 4);
            
            for (int screenIdx = 0; screenIdx < static_cast<int>(this->screenList_.size()); ++screenIdx) {
                tmpA(0, screenIdx) = this->screenList_[screenIdx][bindIdx][0];
                tmpA(1, screenIdx) = this->screenList_[screenIdx][bindIdx][1];
                tmpA(2, screenIdx) = this->screenList_[screenIdx][bindIdx][2];
                tmpA(3, screenIdx) = 1;
            }

            this->bind2Mat_[bindIdx] = CalcMat{ tmpA, tmpD, tmpE };
        }

        this->initW(this->W_, this->screenList_.size());
    }


    void IWAlgo::predInitialRGB(GammaAlgo::IVPType& IVPData) {
        int bindIdx = IVPData.bindIdx_;
        Eigen::MatrixXd& A = this->bind2Mat_[bindIdx].A_;
        Eigen::VectorXd& Wold = (this->bind2gray[bindIdx] == 255) ? this->W_.W255_ : this->W_.Wold_;
        Eigen::VectorXd D_pred = A * Wold;

        IVPData.regValue_.rRegValue_ = static_cast<int>(round(D_pred(0)));
        IVPData.regValue_.gRegValue_ = static_cast<int>(round(D_pred(1)));
        IVPData.regValue_.bRegValue_ = static_cast<int>(round(D_pred(2)));
    }


    void IWAlgo::feedOptimalRGB(const GammaAlgo::IVPType& IVPData) {
        int bindIdx = IVPData.bindIdx_;
        CalcW& W_old = this->W_;
        CalcMat& Mat = this->bind2Mat_[bindIdx];
        Mat.D_ << IVPData.regValue_.rRegValue_, IVPData.regValue_.gRegValue_, IVPData.regValue_.bRegValue_, 1;

        if (this->lamdaList_.empty())
            this->initLamdaList();

        double lamda = this->lamdaList_[bindIdx - 1];

        Eigen::VectorXd tmpW = this->calcW(W_old, Mat, bindIdx, lamda);

        this->updateW(W_old, tmpW, bindIdx);
    }















}











