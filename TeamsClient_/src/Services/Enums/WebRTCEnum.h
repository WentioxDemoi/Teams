#pragma once

/**
 * @enum WebRTCMessageType
 * @brief Types de serveurs utilisés par l'application.
 *
 * Définit les rôles des services backend (authentification, WebRTC, messages)
 * pour distinguer les destinations de communication.
 */
enum class WebRTCMessageType {
    offer,
    answer,
    ice
};